#ifndef HOLOPLAY_MANAGER_H
#define HOLOPLAY_MANAGER_H

#include <godot_cpp/core/class_db.hpp>

#include <godot_cpp/core/binder_common.hpp>

using namespace godot;

class HoloPlayManager : public Object {
	GDCLASS(HoloPlayManager, Object);

public:

protected:
    static HoloPlayManager *singleton;

    static void _bind_methods();
private:

public:
    static HoloPlayManager *get_singleton();

    
    String get_holoplay_core_version();

	HoloPlayManager();
	~HoloPlayManager();
};

#endif // ! HOLOPLAY_MANAGER_H