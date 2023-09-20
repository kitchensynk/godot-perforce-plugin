#ifndef P4PLUGIN_H
#define P4PLUGIN_H

#include "godot_cpp/classes/editor_vcs_interface.hpp"

struct Credentials {
	godot::String username;
	godot::String password;
	godot::String ssh_public_key_path;
	godot::String ssh_private_key_path;
	godot::String ssh_passphrase;
};

namespace godot {

class P4Plugin : public EditorVCSInterface {
    GDCLASS(P4Plugin, EditorVCSInterface)

private:
    

protected:
    static void _bind_methods();

public:
	Credentials creds;

    P4Plugin();
    ~P4Plugin();

	void _set_credentials(const godot::String &username, const godot::String &password, const godot::String &ssh_public_key_path, const godot::String &ssh_private_key_path, const godot::String &ssh_passphrase) override;
	godot::String _get_vcs_name() override;
	bool _initialize(const godot::String &project_path) override;
};

}

#endif