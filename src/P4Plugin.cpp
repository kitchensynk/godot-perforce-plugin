#include "P4Plugin.h"
#include "thirdparty/p4/include/clientapi.h"
#include "thirdparty/p4/include/p4libs.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

class MyClientUser : public ClientUser{
public:
	virtual void OutputInfo( char level, const char * data )
	{      
		godot::UtilityFunctions::print(data);
	}
};

void godot::P4Plugin::_bind_methods() {
	
}

Error e;
godot::P4Plugin::P4Plugin() {
    // Initialize any variables here.
	
	
	
	//UtilityFunctions::print(data);
}

void godot::P4Plugin::_set_credentials(const godot::String &username, const godot::String &password, const godot::String &ssh_public_key_path, const godot::String &ssh_private_key_path, const godot::String &ssh_passphrase) {
	creds.username = username;
	creds.password = password;
	creds.ssh_public_key_path = ssh_public_key_path;
	creds.ssh_private_key_path = ssh_private_key_path;
	creds.ssh_passphrase = ssh_passphrase;
}

godot::String godot::P4Plugin::_get_vcs_name() {
	return "P4";
}

bool godot::P4Plugin::_initialize(const godot::String &project_path) {
	godot::UtilityFunctions::print("INIT");
	
	MyClientUser ui;
    ClientApi client;
    StrBuf msg;
   

    const char* url = "192.168.4.44:1666";
    const char* user = "Jared";
    const char* ws = "Legacy";
    const char* pw = "Cowsaysmoo";

    P4Libraries::Initialize(P4LIBRARIES_INIT_ALL, &e);

    //printf("TEST");

    // Any special protocol mods

    // client.SetProtocol( "tag" );

    // Enable client-side Extensions
    // client.EnableExtensions();

    // Connect to server

    
    client.SetPort(url);
    client.SetUser(user);
    client.SetClient(ws);
    client.SetPassword(pw);
    client.Init(&e);

    //printf("%i",client.GetAPI());
    if (e.Test())
    {
        e.Fmt(&msg);
        fprintf(stderr, "%s\n", msg.Text());
    }else{
	
		UtilityFunctions::print("CONNECTED");
	}

    client.Run("info", &ui);
	
	char data;
	
	ui.OutputInfo('0', &data);

	return true;
}

godot::P4Plugin::~P4Plugin() {
    // Add your cleanup here.
}
