#ifndef P4PLUGIN_H
#define P4PLUGIN_H

#include "godot_cpp/classes/editor_vcs_interface.hpp"
#include "thirdparty/p4/include/clientapi.h"
#include "thirdparty/p4/include/p4libs.h"

#include "godot_cpp/classes/editor_plugin.hpp"
#include "godot_cpp/classes/editor_interface.hpp"
#include "godot_cpp/classes/editor_file_system.hpp"
#include "godot_cpp/classes/editor_file_system_directory.hpp"
#include "godot_cpp/classes/dir_access.hpp"
#include "godot_cpp/classes/project_settings.hpp"

#include <map>



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
		godot::String output;
		std::map<std::string, std::string> output_stat;
		godot::List<godot::String> outputBuffer;
		
		bool debug_mode = false;

        virtual void OutputInfo( char level, const char * data ) override;
		virtual void OutputText( const char * data, int length) override;
		virtual void OutputError( const char * data ) override;
		virtual void OutputStat( StrDict *varList ) override;
		virtual void Prompt(const StrPtr &msg, StrBuf &buf, int noEcho, Error *e) override;	
		bool IsValidASCII(const char * data);
};



namespace godot 
{
	
	class P4FileHandler : public EditorPlugin
	{
		public:
			godot::Callable callable;
			godot::String Init();
			godot::String Refresh();
	};

	class P4Plugin : public EditorVCSInterface 
	{
		GDCLASS(P4Plugin, EditorVCSInterface)

		private:
			godot::TypedArray<godot::Dictionary> modified_files;

		protected:
			static void _bind_methods();

		public:
			//Public VCS Variables
			Credentials creds;
			P4ClientUser ui;
			ClientApi client;
			StrBuf msg;
			
			bool connected = false;
			bool logged_in = false;

			//Constructor/Deconstructor
			P4Plugin();
			~P4Plugin();

			void _connect();

			#pragma region VCS Plugin Methods
			bool _initialize(const godot::String &project_path) override;	
			bool _shut_down() override;
			
			void _set_credentials(const godot::String &username, const godot::String &password, const godot::String &ssh_public_key_path, const godot::String &ssh_private_key_path, const godot::String &ssh_passphrase) override;
			godot::String _get_vcs_name() override;
			godot::TypedArray<godot::Dictionary> _get_modified_files_data() override;

			godot::TypedArray<godot::Dictionary> _get_diff(const godot::String &identifier, int32_t area) override;

			void _pull(const String &remote) override;
			void _push(const String &remote, bool force) override;
			void _fetch(const String &remote) override;
			void _commit(const String &msg) override;
			
			const String defaultCommit = "Submitted by Godot <[o_o]>";
			String commitMessage = "";
			#pragma endregion

			// Testing
			bool _run_p4_command(const char* cmd, char* const* args);	//Cmd w/ multiple args
			bool _run_p4_command(const char* cmd, const char* arg);		//Cmd w/ one arg
			bool _run_p4_command(const char* cmd);						//Cmd w/o args

			void _traverse_file_tree(godot::String path, int depth = 0);

			void _p4_file_change();
	};

}


#endif