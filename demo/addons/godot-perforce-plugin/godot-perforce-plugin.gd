@tool

extends EditorPlugin

var editor
var fs

func _enter_tree():
	print("GD Plugin Init")

	editor = get_editor_interface()
	fs = editor.get_resource_filesystem()
	
	fs.filesystem_changed.connect(_filesystem_changed)
	pass


func _exit_tree():
	print("GD Plugin Close")
	pass

func _filesystem_changed():
	pass
