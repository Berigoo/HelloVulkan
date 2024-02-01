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
#include "HkSurface.h"

class HkDevice;
class XcbSurface : public HkSurface{
private:
    xcb_connection_t* conn;
    xcb_screen_t* screen;
    xcb_window_t windowId;

    std::map<std::string, std::vector<xcb_atom_t>> atoms;
public:
    /// creating essential xcb
    /// \param width
    /// \param height
    /// \param flagsMask or xcb_cw_t
    /// \param flagsEventMask or xcb_event_mask_t
    XcbSurface(uint16_t width, uint16_t height, uint32_t flagsMask, uint32_t flagsEventMask);
    ~XcbSurface();

    /// update window property (by atom type), the atom will be stored on atoms variable
    /// \param property target property
    /// \param changeInto
    /// \param name identity name of this atom (key)
    void updateWindowProperty(std::string property, std::string changeInto, std::string name);
    /// create vulkan surface for xcb
    /// \param instance vulkan instance
    void createSurface(HkDevice *hkDevice);

    void cleanup(VkInstance *pInstance) override;

    VkSurfaceKHR* getSurface() override;
    xcb_connection_t* getConnection();
    xcb_window_t* getWindow();
    VkExtent2D getSurfaceExtent() override;
    std::map<std::string, std::vector<xcb_atom_t>>* getAtoms();
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
