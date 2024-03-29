# Settings list

## Settings

| Long name | Short name | Type | Description |
|:-|:-:|:-:|:-|
| `help` | h | str | Prints a short help message and dumps all the valid variables. |
| `save` || str | Saves all settings to the config file specified by `config_file`. |
| `peer_ip_address` | a | str | IP address of peer to connect to. |
| `peer_network_port` | p | int | Network port of peer to connect to. |
| `network_port` | n | int | Local network port to use. |
| `disable_sdl` | s | bool | Set to disable SDL. This prevents a window from opening. |
| `log_level` | l | int | Set the log level. A range of 0 to 5 is accepted. 0 is disable, 5 is debug. |
| `log_file` || str | Sets the file to log to. |
| `config_compiler_heap_size` || int | Size of the heap that the config script compiler can use. |
| `config_vm_heap_size` || int | Size of the heap that the config VM can use. |
| `config_vm_max_objects` || int | Size of the heap that the config VM can use. Specifically, this is the size of the garbage collected heap. |
| `game_compiler_heap_size` || int | Size of the heap that the game script compiler can use. |
| `game_vm_heap_size` || int | Size of the heap that the game VM can use. |
| `game_vm_max_objects` || int | Size of the heap that the game VM can use. Specifically, this is the size of the garbage collected heap. |
| `autoexec_file` || str | The name of the user file to automatically execute on engine start, if any. |
| `config_file` || str | The name of the machine-generated configuration file to use. |
| `disassemble` || bool | If set, the final bytecode generated by the compiler will be disassembled and printed to the console before execution. |
| `repl` | r | bool | Enables the REPL. |
| `repl_environment` || str | Can have the values "config" or "game", which selects which duck-lisp instance the REPL is connected to. |
| `switch_repl_environment` || str | Toggles `repl_environment` between "config" and "game". |
| `window_width` || str | Sets the width of the main window in pixels. |
| `window_height` || str | Sets the height of the main window in pixels. |

## Duck-lisp Config/REPL Commands

| Name | arguments | Description |
|:-|:-|:-|
| `print` | value::Any | Print `value` to the console. |
| `setting-get` | setting-name::Symbol | Get the value of the given setting. |
| `setting-set` | setting-name::Symbol, value::{Boolean,Integer,String} | Set the value of the given setting to `value`. The type of `value` must match the type of the setting. Floats are not yet supported by duck-lisp. |
