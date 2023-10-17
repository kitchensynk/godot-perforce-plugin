#include "P4Plugin.h"
#include <string.h>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <godot_cpp/core/class_db.hpp>


#pragma region P4ClientUser
//Client User Class Functions
void P4ClientUser::OutputInfo( char level, const char * data )
{
    if(IsValidASCII(data))
    {
        if(debug_mode) 
            godot::UtilityFunctions::print(data);

        output = godot::String(data);
        outputBuffer.push_back(output);
    }
}

void P4ClientUser::OutputError( const char * data )
{
   if(IsValidASCII(data))
    {
        if(debug_mode) 
            godot::UtilityFunctions::push_error(data);

        output = data;
    }
}

void P4ClientUser::OutputText( const char * data, int length )
{
    if(IsValidASCII(data))
    {
        if(debug_mode) 
            godot::UtilityFunctions::print(data);

        output = godot::String(data);
    }
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
#pragma endregion

#pragma region P4FileHandler
godot::String godot::P4FileHandler::Init()
{
    get_editor_interface()->get_resource_filesystem()->connect("filesystem_changed",callable);
    return "";
} 

godot::String godot::P4FileHandler::Refresh()
{
    get_editor_interface()->get_resource_filesystem()->emit_signal("filesystem_changed");
    return "";
} 
#pragma endregion

//Member Variables
//TODO: Move to .h
Error e;

#pragma region P4Plugin
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

void godot::P4Plugin::_bind_methods() 
{ 
    godot::ClassDB::bind_method(godot::D_METHOD("P4FileChange"),&godot::P4Plugin::_p4_file_change);
}

bool godot::P4Plugin::_initialize(const godot::String &project_path) 
{    
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

void godot::P4Plugin::_connect()
{
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
                                "Server Address: " + creds.ssh_public_key_path + "\n" +
                                "Workspace: " + creds.ssh_private_key_path);

    godot::UtilityFunctions::print("\nAttempting to Establish Connection to P4 Server: " + creds.ssh_public_key_path + "...");
    
    //Init Connection
    if(!connected){
        client.Init(&e);
    }
    

    //Connection Success:
    if (e.Test())
    {
        e.Fmt(&msg);
        popup_error("Connection Error:\n" + godot::String(msg.Text()));
        e.Clear();
        
    }else{
        connected = true;
    }

    //Debug Mode = Print statements in godot
    ui.debug_mode = true;
    
    //Use set password and user to login to p4 server
    _run_p4_command("login");

    //Set Login Status
    logged_in = !(ui.output.contains("Password invalid"));
    
    //Bind File-Change Signal   
    P4FileHandler* ps = new P4FileHandler();
    ps->callable = Callable(this,"P4FileChange");
    String callback = ps->Init();
    UtilityFunctions::print(callback);
    
    //Run Methods
    if(logged_in)
    {
        godot::UtilityFunctions::print("\n-------------------------\n");
        godot::UtilityFunctions::print("----    CONNECTED    ----\n");
        godot::UtilityFunctions::print("-------------------------\n");

        
        //Update Changelist if login
        ps->Refresh();
    }
    else
    {
        UtilityFunctions::print("Log-In Failed :(");

        //client.Final(&e);
        //e.Clear();
    }
}

#pragma region VCS Interface Methods
//TODO: Run _connect() from elsewhere
void godot::P4Plugin::_set_credentials(const godot::String &username, const godot::String &password, const godot::String &ssh_public_key_path, const godot::String &ssh_private_key_path, const godot::String &ssh_passphrase)
{
    creds.username = username;
	creds.password = password;
	creds.ssh_public_key_path = ssh_public_key_path;
	creds.ssh_private_key_path = ssh_private_key_path;
	creds.ssh_passphrase = ssh_passphrase;

    ui.creds = creds;

    //Connect to P4 Server
    _connect();
}

//TODO: Better file management for changelist
godot::TypedArray<godot::Dictionary> godot::P4Plugin::_get_modified_files_data()
{   

    //Don't handle files if no P4 connection
    if(!logged_in){
        return modified_files;
    }

    godot::TypedArray<godot::Dictionary> new_modified_files;
        
    //Add changed files to P4 change list
    ui.outputBuffer.clear();
    _run_p4_command("reconcile");

    _run_p4_command("changelist", "-o");

    char* token = strtok(const_cast<char*>(ui.output.utf8().get_data()), "\n");

    bool passed = false;

    while (token != NULL) {
        printf(" % s\n", token);

        godot::String line = godot::String(token);

        if(passed){

            //godot::UtilityFunctions::print("CHANGELIST FILE LINE:");

            ChangeType fileStatus;

            std::string s(line.utf8());
            godot::String delimiter = "#";
            std::string filePath = s.substr(0, line.find(delimiter));
            std::string status = s.substr(line.find(delimiter) + 1, line.length());

            //CLEAR WHITESPACE
            filePath = filePath.erase(0, filePath.find_first_not_of(" \r\n\t\v\f"));
            status = status.erase(0, status.find_first_not_of(" \r\n\t\v\f"));

            filePath = filePath.erase(filePath.find_last_not_of(" \r\n\t\v\f") + 1);
            status.erase(status.find_last_not_of(" \r\n\t\v\f") + 1);

            //godot::UtilityFunctions::print(filePath.c_str());
            //godot::UtilityFunctions::print(status.c_str());

            if(status.find("add") != std::string::npos) {
                fileStatus = CHANGE_TYPE_NEW;
            } 
            if(status.find("edit") != std::string::npos) {
                fileStatus = CHANGE_TYPE_MODIFIED;
            }
            if(status.find("delete") != std::string::npos) {
                fileStatus = CHANGE_TYPE_DELETED;
            } 
            
            //godot::UtilityFunctions::print("Getting global path for file...");
            _run_p4_command("where", filePath.c_str());
            std::string s2((ui.output.utf8().get_data()));
            if(ui.output.contains(":"))
            {   
                std::string globalPath = s2.substr(ui.output.find(":") -1);
                //godot::UtilityFunctions::print(globalPath.c_str());

                new_modified_files.push_back(create_status_file(ProjectSettings::get_singleton()->globalize_path(String(globalPath.c_str())), fileStatus, TREE_AREA_UNSTAGED));  
            }
            else
                godot::UtilityFunctions::print("no good boss");
        }

        if(line.contains("Files:") && !line.contains(" Files:")){ 
            passed = true;
        }
        token = strtok(NULL, "\n");
    }

    //IMPORTANT
    ui.outputBuffer.clear();

    return new_modified_files;
}

godot::String godot::P4Plugin::_get_vcs_name() 
{
	return "P4";
}

void godot::P4Plugin::_pull(const String &remote) 
{
    _run_p4_command("sync");
}

void godot::P4Plugin::_fetch(const String &remote) 
{
    _run_p4_command("sync");
}

void godot::P4Plugin::_commit(const String &msg)
{
    commitMessage = msg;
}

//TODO: Add force push
void godot::P4Plugin::_push(const String &remote, bool force)
{
    std::string msg = ((commitMessage == "") ? defaultCommit : commitMessage + "\"").utf8();
    //_run_p4_command("submit", msg.utf8());
    //_run_p4_command("submit", "-d \"Submitted by Godot <[o_o]>\"");

    char* bs[] = {"-d", const_cast<char*>(msg.c_str())};
    client.SetArgv(2, bs);  
    client.Run("submit", &ui);

}
#pragma endregion VCS Interface Methods

/// @brief Event fires when files are added/deleted/modified in godot
void godot::P4Plugin::_p4_file_change()
{
    godot::UtilityFunctions::print("File Changed");

    //_get_modified_files_data();
}

//Testing Methods
bool godot::P4Plugin::_run_p4_command(const char* cmd, const char* arg)
{
    char* bs[] = {const_cast<char*>(arg)};
    client.SetArgv(1, bs);  
    client.Run(cmd, &ui);

    //char data;
    //ui.OutputInfo('0', &data);
    //ui.OutputError(&data);
    //ui.OutputText(&data, 1000);  
   
    return true;
}

bool godot::P4Plugin::_run_p4_command(const char* cmd)
{
    client.Run(cmd, &ui);

    //char data;
    //ui.OutputInfo('0', &data);
    //ui.OutputError(&data);
    //ui.OutputText(&data, 1000);  
   
    return true;
}

void godot::P4Plugin::_traverse_file_tree(godot::String path, int depth)
{
    //Safegaurd
    if(depth > 10)
        return;

    PackedStringArray childDir = DirAccess::open(path)->get_directories();

    for(int i = 0; i < childDir.size(); i++)
    {   
        godot::String str = godot::String(std::string(depth, '-').c_str());        
        //UtilityFunctions::print(str + "+ " + childDir[i] + "/");

        String newpath = path + childDir[i] + "/";
        PackedStringArray files = DirAccess::open(path)->get_files_at(newpath);

        for(int j = 0; j < files.size(); j++)
        {
            //godot::String str = godot::String(std::string(depth+1, '-').c_str());
            //UtilityFunctions::print(str + "o " + files[j]);
            
            std::string filePath = ProjectSettings::get_singleton()->globalize_path(newpath + files[j]).utf8();            
            _run_p4_command("status", filePath.c_str());

            //P4Plugin::RunP4Command("status", bs);
        }
        
        godot::P4Plugin::_traverse_file_tree(newpath, depth + 1);
    }
}

#pragma endregion