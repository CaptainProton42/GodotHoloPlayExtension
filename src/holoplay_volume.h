#ifndef HOLOPLAY_VOLUME_H
#define HOLOPLAY_VOLUME_H

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/core/defs.hpp>

#include <godot_cpp/core/binder_common.hpp>

using namespace godot;

class HoloPlayVolume : public Node3D {
	GDCLASS(HoloPlayVolume, Node3D);

private:
	int internal_device_index; // Device index used by the HoloPlayCore API.
	int managed_device_index = 0; // Device index used by the HoloPlayManager.
	bool headless = false;

	int window_id = 0;

protected:
	static void _bind_methods();

private:
	void _set_headless(bool p_headless);
	bool _is_headless() const;

public:
	void set_device_index(int p_device_index);
	int get_device_index() const;

	HoloPlayVolume();
};

#endif // ! HOLOPLAY_VOLUME_H