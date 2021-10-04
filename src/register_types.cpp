#include "register_types.h"

#include <godot/gdnative_interface.h>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/godot.hpp>

#include <string>

#include "holoplay_manager.h"
#include "holoplay_volume.h"

using namespace godot;

static HoloPlayManager *holoplay_manager = nullptr;

void register_types() {
	ClassDB::register_class<HoloPlayVolume>();
	ClassDB::register_class<HoloPlayManager>();

	holoplay_manager = memnew(HoloPlayManager());

	Engine::get_singleton()->register_singleton("HoloPlayManager", HoloPlayManager::get_singleton());
}

void unregister_types() {}

extern "C" {

// Initialization.

GDNativeBool GDN_EXPORT holoplay_library_init(const GDNativeInterface *p_interface, const GDNativeExtensionClassLibraryPtr p_library, GDNativeInitialization *r_initialization) {
	godot::GDExtensionBinding::InitObject init_obj(p_interface, p_library, r_initialization);

	init_obj.register_scene_initializer(register_types);
	init_obj.register_scene_terminator(unregister_types);

	return init_obj.init();
}
}