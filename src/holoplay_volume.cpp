#include "holoplay_volume.h"

#include <godot_cpp/core/class_db.hpp>

#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/display_server.hpp>

#include "holoplay_manager.h"

#include "HoloPlayCore.h"

using namespace godot;

void HoloPlayVolume::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_device_index"), &HoloPlayVolume::get_device_index);
    ClassDB::bind_method(D_METHOD("set_device_index", "device_index"), &HoloPlayVolume::set_device_index);
    // Properties.
    ADD_PROPERTY(PropertyInfo(Variant::INT, "device_index"), "set_device_index", "get_device_index");
}

bool HoloPlayVolume::_is_headless() const {
    return headless;
}

void HoloPlayVolume::_set_headless(bool p_headless) {
    if (p_headless) {
        if (window_id) {
            DisplayServer::get_singleton()->delete_sub_window(window_id);
            window_id = 0;
        }
    } else {
        if (!window_id) {
            uint32_t flags = DisplayServer::WINDOW_FLAG_BORDERLESS | DisplayServer::WINDOW_FLAG_ALWAYS_ON_TOP | DisplayServer::WINDOW_FLAG_NO_FOCUS;
            int win_x = hpc_GetDevicePropertyWinX(internal_device_index);
            int win_y = hpc_GetDevicePropertyWinY(internal_device_index);
            int width = hpc_GetDevicePropertyScreenW(internal_device_index);
            int height = hpc_GetDevicePropertyScreenH(internal_device_index);
            window_id = DisplayServer::get_singleton()->create_sub_window(DisplayServer::WINDOW_MODE_WINDOWED, DisplayServer::VSYNC_DISABLED, 0, Rect2(win_x, win_y, width, height));
        }
    }
    headless = p_headless;
}

void HoloPlayVolume::set_device_index(int p_device_index) {
    Array connected_devices = HoloPlayManager::get_singleton()->get_connected_devices();
    if (p_device_index + 1 > connected_devices.size()) {
        _set_headless(true);
        managed_device_index = p_device_index;
        return;
    }

    internal_device_index = connected_devices.slice(p_device_index, p_device_index).front(); // [] operator is not implemented yet.
    _set_headless(false);
    managed_device_index = p_device_index;
}


int HoloPlayVolume::get_device_index() const {
    return managed_device_index;
}

HoloPlayVolume::HoloPlayVolume() { }