# SEScript Reference

Scripts live in `scripts/*.ses` and attach to objects via `script_file` in the scene.

## Lifecycle
- `func _ready():` — once on load
- `func _update(dt):` — each frame
- `if Input.key_down("W"):` — conditional (next line)

## Object
- `self.translate(x, y, z)`
- `self.set_position(x, y, z)`
- `self.delete()`
- `var x = Scene.find("Name")`

## Scene
- `Scene.spawn_entity("Name")`
- `Scene.delete("Name")`
- `Scene.set_camera("Name")`

## Files
- `FS.write("path", "text")`
- `FS.exists("path")`

## Misc
- `log("message")`
- `Engine.quit()`
