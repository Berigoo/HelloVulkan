//
// Created by pach on 12/31/23.
//

#include "XcbSurface.h"
#include "HkDevice.h"

XcbSurface::XcbSurface(uint16_t width, uint16_t height, uint32_t flagsMask, uint32_t flagsEventMask) {
    conn = xcb_connect(nullptr, nullptr);
    if(xcb_connection_has_error(conn)) throw std::runtime_error("error connecting to X server");

    screen = xcb_setup_roots_iterator(xcb_get_setup(conn)).data;
    windowId = xcb_generate_id(conn);

    uint32_t valueMask = flagsMask;
    uint32_t valueList[2];
    valueList[0] = screen->black_pixel;
    valueList[1] = flagsEventMask;

    xcb_create_window(conn, screen->root_depth, windowId, screen->root, 0, 0, width, height, 1, XCB_WINDOW_CLASS_INPUT_OUTPUT, screen->root_visual, valueMask, valueList);

    xcb_map_window(conn, windowId);
    xcb_flush(conn);
}

xcb_intern_atom_cookie_t XcbSurface::internAtomCookie(const std::string &str) {
    return xcb_intern_atom(conn, false, str.size(), str.c_str());
}

xcb_atom_t XcbSurface::internAtom(xcb_intern_atom_cookie_t cookie) {
    xcb_atom_t atom = XCB_ATOM_NONE;
    xcb_intern_atom_reply_t* reply = xcb_intern_atom_reply(conn, cookie, nullptr);
    if(reply){
        atom = reply->atom;
        free(reply);
    }
    return atom;
}

void XcbSurface::updateWindowProperty(std::string property, std::string changeInto, std::string name) {
    xcb_intern_atom_cookie_t propCookie = internAtomCookie(property);
    xcb_atom_t propAtom = internAtom(propCookie);
    xcb_intern_atom_cookie_t chgCookie = internAtomCookie(changeInto);
    xcb_atom_t chgAtom = internAtom(chgCookie);

    std::vector<xcb_atom_t> tmp(2);
    tmp[0] = propAtom;
    tmp[1] = chgAtom;
    // TODO add exception for multiple same name
    atoms.insert(std::pair<std::string, std::vector<xcb_atom_t>>(name, tmp));

    xcb_change_property(conn, XCB_PROP_MODE_REPLACE, windowId, atoms.at(name)[0], XCB_ATOM_ATOM, 32, 1, &atoms.at(name)[1]);
}

void XcbSurface::createSurface(HkDevice *hkDevice) {
    VkXcbSurfaceCreateInfoKHR info{};
    info.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
    info.connection = conn;
    info.window = windowId;
    if(vkCreateXcbSurfaceKHR(*hkDevice->getInstance(), &info, nullptr, &surface) != VK_SUCCESS){
        throw std::runtime_error("failed create xcb surface!");
    }
}

VkSurfaceKHR *XcbSurface::getSurface() {
    if(surface) return &surface;
    else{
        spdlog::warn("surface null!");
        return nullptr;
    }
}

xcb_connection_t *XcbSurface::getConnection() {
    return conn;
}

xcb_window_t *XcbSurface::getWindow() {
    return &windowId;
}

VkExtent2D XcbSurface::getSurfaceExtent() {
    VkExtent2D extent = {
            0,
            0
    };
    xcb_get_geometry_reply_t* reply;
    auto cookie = xcb_get_geometry(conn, windowId);
    if((reply = xcb_get_geometry_reply(conn, cookie, nullptr))){
        extent = {
                static_cast<uint32_t>(reply->width),
                static_cast<uint32_t>(reply->height)
        };
        spdlog::debug("resized to {} {}", reply->width, reply->height);
        free(reply);

        return extent;
    }
    spdlog::warn("cannot get geometry reply");
    return extent;
}

XcbSurface::~XcbSurface() {

}

void XcbSurface::cleanup(VkInstance *pInstance) {
    HkSurface::cleanup(pInstance);
    xcb_destroy_window(conn, windowId);
    xcb_disconnect(conn);
}

std::map<std::string, std::vector<xcb_atom_t>> *XcbSurface::getAtoms() {
    return &atoms;
}
