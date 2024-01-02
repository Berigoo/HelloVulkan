//
// Created by pach on 12/31/23.
//

#ifndef HAKUREIENGINE_XCBSURFACE_H
#define HAKUREIENGINE_XCBSURFACE_H
#define VK_USE_PLATFORM_XCB_KHR
#include <vulkan/vulkan.h>
#include <xcb/xcb.h>
#include <utility>
#include <stdexcept>
#include <map>
#include <spdlog/spdlog.h>

class XcbSurface {
private:
    xcb_connection_t* conn;
    xcb_screen_t* screen;
    xcb_window_t windowId;

    VkSurfaceKHR surface;

    std::map<std::string, xcb_atom_t> atoms;
public:
    /// creating essential xcb
    /// \param width
    /// \param height
    /// \param flagsMask or xcb_cw_t
    /// \param flagsEventMask or xcb_event_mask_t
    XcbSurface(uint16_t width, uint16_t height, uint32_t flagsMask, uint32_t flagsEventMask);

    /// update window property (by atom type), the atom will be stored on atoms variable
    /// \param property target property
    /// \param changeInto
    /// \param name identity name of this atom (key)
    void updateWindowProperty(std::string property, std::string changeInto, std::string name);
    /// create vulkan surface for xcb
    /// \param instance vulkan instance
    void createSurface(VkInstance instance);

    VkSurfaceKHR* getSurface();
private:
    /// retrieve atom cookie
    /// \param str the name of atom (ex: WM_DELETE_WINDOW, WM_PROTOCOLS)
    /// \return
    xcb_intern_atom_cookie_t internAtomCookie(const std::string &str);
    /// retrieve atom
    /// \param cookie atom cookie
    /// \return
    xcb_atom_t internAtom(xcb_intern_atom_cookie_t cookie);
};


#endif //HAKUREIENGINE_XCBSURFACE_H
