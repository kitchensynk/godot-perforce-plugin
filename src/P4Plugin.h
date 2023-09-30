#ifndef P4PLUGIN_H
#define P4PLUGIN_H

#include "godot_cpp/classes/editor_vcs_interface.hpp"
#include "thirdparty/p4/include/clientapi.h"
#include "thirdparty/p4/include/p4libs.h"

#include "godot-cpp/gen/include/godot_cpp/classes/editor_plugin.hpp"
#include "godot-cpp/gen/include/godot_cpp/classes/editor_interface.hpp"
#include "godot-cpp/gen/include/godot_cpp/classes/editor_file_system.hpp"

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
		Credentials creds;

        virtual void OutputInfo( char level, const char * data ) override;
		virtual void OutputText( const char * data, int length) override;
		virtual void OutputError( const char * data ) override;
		virtual void Prompt(const StrPtr &msg, StrBuf &buf, int noEcho, Error *e) override;	
		bool IsValidASCII(const char * data);
};

class P4FileHandler : godot::EditorPlugin
{
	GDCLASS(P4FileHandler, godot::EditorPlugin);

	public:
		godot::EditorInterface interface;
		godot::EditorFileSystem fileSystem;
		
		P4FileHandler();
		~P4FileHandler();
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
			godot::TypedArray<godot::Dictionary> _get_modified_files_data() override;

			// Testing
			void _test();	
			bool RunP4Command(const char * cmd, char* const* args);
	};

}

#endif