#include "P4Plugin.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

       
void P4ClientUser::OutputInfo( char level, const char * data )
{
    godot::UtilityFunctions::print(data);
}

//Member Variables
//TODO: Move to .h
Error e;


//Plugin
godot::P4Plugin::P4Plugin() 
{
    godot::UtilityFunctions::print("Plugin Init");
}

godot::P4Plugin::~P4Plugin() 
{
    godot::UtilityFunctions::print("Plugin Shutdown");
    e.Clear();
    P4Libraries::Shutdown(P4LIBRARIES_INIT_ALL, &e);
}

//VCS
bool godot::P4Plugin::_initialize(const godot::String &project_path) {
	godot::UtilityFunctions::print("Initializing P4 Plugin");
	return true;
}

bool godot::P4Plugin::_shut_down()
{
    godot::UtilityFunctions::print("VCS Shutdown");
    client.Final(&e);
    
    P4Libraries::Shutdown(P4LIBRARIES_INIT_ALL, &e);
    return true;
}

//Override Methods
void godot::P4Plugin::_bind_methods() { }

void godot::P4Plugin::_set_credentials(const godot::String &username, const godot::String &password, const godot::String &ssh_public_key_path, const godot::String &ssh_private_key_path, const godot::String &ssh_passphrase)
{
    creds.username = username;
	creds.password = password;
	creds.ssh_public_key_path = ssh_public_key_path;
	creds.ssh_private_key_path = ssh_private_key_path;
	creds.ssh_passphrase = ssh_passphrase;

    client.Clear();
    client.Reset();
    //client.~ClientApi();
    P4Libraries::Shutdown(P4LIBRARIES_INIT_ALL, &e);

    //Testing
    P4Libraries::Initialize(P4LIBRARIES_INIT_ALL, &e);

    // Connect to server using EditorVCS Window
    // NOTE: use SSH Public/Private paths for Server Address/WS name 
    client.SetUser(creds.username.utf8());
    client.SetPassword(creds.password.utf8());
    client.SetPort(creds.ssh_public_key_path.utf8());
    client.SetClient(creds.ssh_private_key_path.utf8());
    
    UtilityFunctions::print("\nCurrent VCS Credentials:");
    UtilityFunctions::print("User: " + creds.username + "\n" +
                            "Server Address: " + creds.ssh_public_key_path + "\n" +
                            "Workspace: " + creds.ssh_private_key_path);

    UtilityFunctions::print("\n Attempting to Establish Connection to P4 Server: " + creds.ssh_public_key_path + "...");
    
    //Init Connection
    client.Init(&e);

    //Connection Success:
    if (e.Test())
    {
        e.Fmt(&msg);
        popup_error("Connection Error:\n" + godot::String(msg.Text()));
        e.Clear();
    }
    else
    {
		UtilityFunctions::print("\n-------------------------\n");
        UtilityFunctions::print("----    CONNECTED    ----\n");
        UtilityFunctions::print("-------------------------\n");
	}

    //Debug Testing:
    client.Run("info", &ui);
	
	char data;	
	ui.OutputInfo('0', &data);

    //Cleanup
    client.Final(&e);
    e.Clear();
}

godot::String godot::P4Plugin::_get_vcs_name() 
{
	return "P4";
}



