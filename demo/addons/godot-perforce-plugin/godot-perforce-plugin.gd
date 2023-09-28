@tool

extends EditorPlugin

func _enter_tree():
    print("GD Plugin Init");
    pass


func _exit_tree():
    print("GD Plugin Close");
    pass