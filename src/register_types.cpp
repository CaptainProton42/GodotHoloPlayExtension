#include "register_types.h"

#include <godot/gdnative_interface.h>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

#include <string>

#include "HoloPlayCore.h"

#include "holoplay_volume.h"

using namespace godot;

void register_types() {
	ClassDB::register_class<HoloPlayVolume>();
}

void unregister_types() {}

extern "C" {

// Initialization.

GDNativeBool GDN_EXPORT holoplay_library_init(const GDNativeInterface *p_interface, const GDNativeExtensionClassLibraryPtr p_library, GDNativeInitialization *r_initialization) {
	godot::GDExtensionBinding::InitObject init_obj(p_interface, p_library, r_initialization);

	init_obj.register_scene_initializer(register_types);
	init_obj.register_scene_terminator(unregister_types);

	hpc_client_error errco = hpc_InitializeApp("Godot HoloPlay Plugin", hpc_LICENSE_NONCOMMERCIAL);
	if (errco) {
		std::string errstr;
		switch (errco)
		{
			case hpc_CLIERR_NOSERVICE:
				errstr = "HoloPlay Service not running";
				break;
			case hpc_CLIERR_SERIALIZEERR:
				errstr = "Client message could not be serialized";
				break;
			case hpc_CLIERR_VERSIONERR:
				errstr = "Incompatible version of HoloPlay Service";
				break;
			case hpc_CLIERR_PIPEERROR:
				errstr = "Interprocess pipe broken";
				break;
			case hpc_CLIERR_SENDTIMEOUT:
				errstr = "Interprocess pipe send timeout";
				break;
			case hpc_CLIERR_RECVTIMEOUT:
				errstr = "Interprocess pipe receive timeout";
				break;
			default:
				errstr = "Unknown error";
				break;
		}

		ERR_PRINT(("Could not connect to the HoloPlay service: " + errstr + " (" + std::to_string(errco) + ").").c_str());
		//return;
	}

	return init_obj.init();
}
}