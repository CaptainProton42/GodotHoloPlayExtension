#ifndef HOLOPLAY_MANAGER_H
#define HOLOPLAY_MANAGER_H

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/binder_common.hpp>

#include <godot_cpp/classes/rd_shader_spirv.hpp>


using namespace godot;

class HoloPlayManager : public Object {
	GDCLASS(HoloPlayManager, Object);

public:
	enum LicenseType {
        LICENSE_TYPE_NONCOMMERCIAL,
        LICENSE_TYPE_COMMERCIAL
    };
    
private:
    Ref<RDShaderSPIRV> _lightfield_shader;

protected:
    static HoloPlayManager *singleton;

    static void _bind_methods();

private:
    void _compile_shaders();

public:
    static HoloPlayManager *get_singleton();

    bool initialize_app(String name, int license = LICENSE_TYPE_NONCOMMERCIAL);
    Array get_connected_devices();
    String get_holoplay_core_version();

	HoloPlayManager();
	~HoloPlayManager();
};

VARIANT_ENUM_CAST(HoloPlayManager, HoloPlayManager::LicenseType);

#endif // ! HOLOPLAY_MANAGER_H