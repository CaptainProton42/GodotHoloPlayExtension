#include "holoplay_manager.h"

#include <godot_cpp/core/class_db.hpp>

#include "HoloPlayCore.h"

#include <string>

HoloPlayManager *HoloPlayManager::singleton = nullptr;

HoloPlayManager *HoloPlayManager::get_singleton() {
	return singleton;
};

void HoloPlayManager::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_holoplay_core_version"), &HoloPlayManager::get_holoplay_core_version);
}

String HoloPlayManager::get_holoplay_core_version() {
    char buf[64];
    size_t written = hpc_GetHoloPlayCoreVersion(buf, 64);
    if (written) ERR_PRINT("Preallocated string too small for hpc_GetHoloPlayCoreVersion.");
    return String(buf);
}

HoloPlayManager::HoloPlayManager() {
    singleton = this;

    // NOTE: Maybe we should consider moving this into a public function that has to be explicitly called
    // by the user? This would allow for better error handling and allow the user to supply a custom app name and license.
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
		return;
	}
}

HoloPlayManager::~HoloPlayManager() {
    hpc_CloseApp();

    singleton = nullptr;
}