#ifndef HOLOPLAY_VOLUME_H
#define HOLOPLAY_VOLUME_H

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/classes/viewport.hpp>

#include <godot_cpp/core/binder_common.hpp>

using namespace godot;

class HoloPlayVolume : public Node3D {
	GDCLASS(HoloPlayVolume, Node3D);

protected:
	static void _bind_methods();

private:

public:

};

#endif // ! HOLOPLAY_VOLUME_H