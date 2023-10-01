#include "P4Plugin.h"
#include <string.h>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <godot_cpp/core/class_db.hpp>

//Client User Class Functions
void P4ClientUser::OutputInfo( char level, const char * data )
{
    if(IsValidASCII(data))
        godot::UtilityFunctions::print(data);
}
void P4ClientUser::OutputError( const char * data )
{
    if(IsValidASCII(data))
        godot::UtilityFunctions::push_error(data);
}

void P4ClientUser::OutputText( const char * data, int length )
{
    if(IsValidASCII(data))
        godot::UtilityFunctions::print(data);
}

/// @brief This method is automatically called by P4API when a Run() requires user input. Currently just used for Run("login")
/// @param msg 
/// @param buf 
/// @param noEcho 
/// @param e 
void P4ClientUser::Prompt( const StrPtr &msg, StrBuf &buf, int noEcho, Error *e )
{ 
    if(strcmp(msg.Text(),"Enter password:"))
    {
        buf.Set(creds.password.utf8());
    }
    else
    {
        godot::UtilityFunctions::push_error(msg.Text());
        godot::UtilityFunctions::push_error("^ PROMPT NOT IMPLEMENTED ^"); 
    }
}

bool P4ClientUser::IsValidASCII(const char * data)
{
    std::string s = static_cast<std::string>(data);
    
    if(s.length() == 0)
        return false;

    for (auto c: s) 
    {
        if (static_cast<unsigned char>(c) > 127) 
            return false;
    }

    return true;
}

void godot::P4FileHandler::Init(){
    get_editor_interface()->get_resource_filesystem()->connect("filesystem_changed",callable);
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
void godot::P4Plugin::_bind_methods() { 
    godot::ClassDB::bind_method(godot::D_METHOD("P4FileChange"),&godot::P4Plugin::P4FileChange);
}

void godot::P4Plugin::P4FileChange(){
    godot::UtilityFunctions::print("File Changed");
}

void godot::P4Plugin::_test()
{

        //P4FileHandler* ps = new P4FileHandler();

        

        //godot::Callable callable = Callable();

        //testclass* tc = new testclass();

        //godot::Callable callable = Callable(this,"testprint");
        

        //ps->get_editor_interface()->get_resource_filesystem()->connect("filesystem_changed",callable);

        //ps->Init();
        //Debug Testing:

        //client.Run("info", &ui);

        //char data;
        //char* arg[] = {"D:\\GamedevWorkspaces\\Legacy\\test.txt"};
        //client.SetArgv(1,arg);
        //client.Run("resolve", &ui);

        //godot::UtilityFunctions::print("\np4 files :");
        //char* bs[] = {"//depot/..."};
        //client.SetArgv(1, bs);
        //client.Run("files", &ui);

        //char data;
        //ui.OutputInfo('0', &data);
        //ui.OutputError(&data);
        //ui.OutputText(&data, 1000);  
        
        //char* bs[] = {"//depot/..."};
        //RunP4Command("files", bs);

        char* args[] = {"-sa"};
        RunP4Command("diff", args);  
}

bool godot::P4Plugin::RunP4Command(const char* cmd, char* const* args)
{
    client.SetArgv(sizeof(args), args);  
    client.Run(cmd, &ui);

    char data;
    ui.OutputInfo('0', &data);
    ui.OutputError(&data);
    ui.OutputText(&data, 1000);  

    return true;
}

void godot::P4Plugin::_set_credentials(const godot::String &username, const godot::String &password, const godot::String &ssh_public_key_path, const godot::String &ssh_private_key_path, const godot::String &ssh_passphrase)
{
    
    creds.username = username;
	creds.password = password;
	creds.ssh_public_key_path = ssh_public_key_path;
	creds.ssh_private_key_path = ssh_private_key_path;
	creds.ssh_passphrase = ssh_passphrase;

    ui.creds = creds;

    //Initalize P4
    P4Libraries::Initialize(P4LIBRARIES_INIT_ALL, &e);
    if (e.Test())
    {
        e.Fmt(&msg);
        popup_error("P4 Error:\n" + godot::String(msg.Text()));
        e.Clear();
    }

    // Connect to server using EditorVCS Window
    // NOTE: use SSH Public/Private paths for Server Address/WS name 
    client.SetUser(creds.username.utf8());
    
    client.SetPort(creds.ssh_public_key_path.utf8());
    client.SetClient(creds.ssh_private_key_path.utf8());

    godot::UtilityFunctions::print("\nCurrent VCS Credentials:");
    godot::UtilityFunctions::print("User: " + creds.username + "\n" +
                            "Password: " + creds.password + "\n" +
                            "Server Address: " + creds.ssh_public_key_path + "\n" +
                            "Workspace: " + creds.ssh_private_key_path);

    godot::UtilityFunctions::print("\nAttempting to Establish Connection to P4 Server: " + creds.ssh_public_key_path + "...");
    
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
        godot::UtilityFunctions::print("\n-------------------------\n");
        godot::UtilityFunctions::print("----    CONNECTED    ----\n");
        godot::UtilityFunctions::print("-------------------------\n");
    }

    //Use set password and user to login to p4 server
    client.Run("login", &ui);

    //
   
    P4FileHandler* ps = new P4FileHandler();

    ps->callable = Callable(this,"P4FileChange");

    ps->Init();

    _test();

    //Cleanup
    client.Run("logout", &ui);

    client.Final(&e);
    e.Clear();
}

godot::String godot::P4Plugin::_get_vcs_name() 
{
	return "P4";
}

godot::TypedArray<godot::Dictionary> godot::P4Plugin::_get_modified_files_data()
{

    return godot::TypedArray<godot::Dictionary>();
}
