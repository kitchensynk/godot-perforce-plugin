#ifndef P4PLUGIN_H
#define P4PLUGIN_H

#include "godot_cpp/classes/editor_vcs_interface.hpp"
#include "thirdparty/p4/include/clientapi.h"
#include "thirdparty/p4/include/p4libs.h"

struct Credentials {
	godot::String username;
	godot::String password;
	godot::String ssh_public_key_path;
	godot::String ssh_private_key_path;
	godot::String ssh_passphrase;
};

class P4ClientUser : public ClientUser
{
    public:
        virtual void OutputInfo( char level, const char * data ) override;
       
};


namespace godot 
{
	class P4Plugin : public EditorVCSInterface 
	{
		GDCLASS(P4Plugin, EditorVCSInterface)

		protected:
			static void _bind_methods();

		public:

			//Public VCS Variables
			Credentials creds;
			P4ClientUser ui;
			ClientApi client;
			StrBuf msg;

			//Constructor/Deconstructor
			P4Plugin();
			~P4Plugin();

			//VCS Plugin Methods
			bool _initialize(const godot::String &project_path) override;	
			bool _shut_down() override;
			
			void _set_credentials(const godot::String &username, const godot::String &password, const godot::String &ssh_public_key_path, const godot::String &ssh_private_key_path, const godot::String &ssh_passphrase) override;
			godot::String _get_vcs_name() override;

			
	};

}

#endif