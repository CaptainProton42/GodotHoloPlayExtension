#include "holoplay_manager.h"

#include <godot_cpp/core/class_db.hpp>

#include <godot_cpp/classes/rendering_device.hpp>
#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/classes/rd_shader_source.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/global_constants.hpp>

#include "HoloPlayCore.h"
#include "HoloPlayShadersOpen.h"

#include <string>

HoloPlayManager *HoloPlayManager::singleton = nullptr;

HoloPlayManager *HoloPlayManager::get_singleton() {
	return singleton;
};

void HoloPlayManager::_bind_methods() {
	ClassDB::bind_method(D_METHOD("initialize_app", "app_name", "license_type"), &HoloPlayManager::initialize_app);
    ClassDB::bind_method(D_METHOD("get_holoplay_core_version"), &HoloPlayManager::get_holoplay_core_version);
	ClassDB::bind_method(D_METHOD("get_connected_devices"), &HoloPlayManager::get_connected_devices);

	BIND_ENUM_CONSTANT(LICENSE_TYPE_NONCOMMERCIAL);
	BIND_ENUM_CONSTANT(LICENSE_TYPE_COMMERCIAL);
}

void HoloPlayManager::_compile_shaders() {
	// The preamble of the HoloPlayCore SDK shader source has to be replaced in order to
	// allow compilation to SPIRV.

	std::string frag_source = "\
#version 450 core\n\
layout(location = 1) in vec2 texCoords;\n\
layout(location = 2) out vec4 fragColor;\n\
\n\
layout(binding = 0) uniform parameters {\n\
// Calibration values\n\
float pitch;\n\
float tilt;\n\
float center;\n\
int invView;\n\
float subp;\n\
float displayAspect;\n\
int ri;\n\
int bi;\n\
\n\
// Quilt settings\n\
vec3 tile;\n\
vec2 viewPortion;\n\
float quiltAspect;\n\
int overscan;\n\
int quiltInvert;\n\
\n\
int debug;\n\
};\n\
\n\
layout(binding = 1) uniform sampler2D screenTex;\n\
\n\
";

	std::string vert_source = "\
#version 450 core\n\
layout (location = 0) in vec2 pos;\n\
layout (location = 1) out vec2 texCoords;\n\
\n\
void main()\n\
{\n\
	gl_Position = vec4(pos.xy, 0.0, 1.0);\n\
	texCoords = (pos.xy + 1.0) * 0.5;\n\
}";

	// For licensing reasons, the "body" of the fragment shader needs to be loaded from the dynlib.
	frag_source.append(hpc_LightfieldFragShaderGLSL + 416);

	Ref<RDShaderSource> shader_source;
	shader_source.instantiate();

	shader_source->set_stage_source(RenderingDevice::SHADER_STAGE_VERTEX, String(vert_source.c_str()));
	shader_source->set_stage_source(RenderingDevice::SHADER_STAGE_FRAGMENT, String(frag_source.c_str()));

	RenderingDevice *rendering_device = RenderingServer::get_singleton()->get_rendering_device();
	Ref<RDShaderSPIRV> lightfield_shader = rendering_device->shader_compile_spirv_from_source(shader_source);

	String vert_compile_error = lightfield_shader->get_stage_compile_error(RenderingDevice::SHADER_STAGE_VERTEX);
	String frag_compile_error = lightfield_shader->get_stage_compile_error(RenderingDevice::SHADER_STAGE_FRAGMENT);

	if (vert_compile_error.length() > 0) {
		UtilityFunctions::printerr("Lightfield shader vertex stage compilation error:\n" + vert_compile_error);
		return;
	} else if (frag_compile_error.length() > 0) {
		UtilityFunctions::printerr("Lightfield shader fragment stage compilation error:\n" + frag_compile_error);
		return;
	}

	_lightfield_shader = lightfield_shader;
}

bool HoloPlayManager::initialize_app(String name, int license) {
	_compile_shaders();
	if (!_lightfield_shader.ptr()) return false;

	_hpc_license_type hpc_license;
	switch (license) {
		case LICENSE_TYPE_COMMERCIAL:
			hpc_license = hpc_LICENSE_COMMERCIAL;
			break;
		case LICENSE_TYPE_NONCOMMERCIAL:
		default:
			hpc_license = hpc_LICENSE_NONCOMMERCIAL;
			break;
	}

	// NOTE: Maybe we should consider moving this into a public function that has to be explicitly called
    // by the user? This would allow for better error handling and allow the user to supply a custom app name and license.
    hpc_client_error errco = hpc_InitializeApp(name.ascii().get_data(), hpc_license);
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
		return false;
	}

	return true;
}

Array HoloPlayManager::get_connected_devices() {
	int num_devices = hpc_GetNumDevices();
	Array devices;
	// There is no guarantee that devices have consecutive indices, so we can't just loop
	// over all indices until we reach num_devices. Instead, we loop over a slightly larger
	// number and stop when we have found enough devices.
	for (int i = 0; i < 2*num_devices; ++i) {
		char buf[64];
		size_t err_size = hpc_GetDeviceType(i, buf, 64);
		if (err_size) ERR_PRINT("Preallocated string too small for hpc_GetDeviceType.");
		if (buf && buf[0]) { // if buffer is not empty.
			devices.push_back(i);
			if (devices.size() == num_devices) break;
		}
	}

	return devices;
}

String HoloPlayManager::get_holoplay_core_version() {
    char buf[64];
    size_t err_size = hpc_GetHoloPlayCoreVersion(buf, 64);
    if (err_size) ERR_PRINT("Preallocated string too small for hpc_GetHoloPlayCoreVersion.");
    return String(buf);
}

HoloPlayManager::HoloPlayManager() {
    singleton = this;
}

HoloPlayManager::~HoloPlayManager() {
    hpc_CloseApp();

    singleton = nullptr;
}