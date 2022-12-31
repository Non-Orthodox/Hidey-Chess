# Configuration

## CLI

Options can be given on the command line. A list of game settings can be found in `settings.md`. Either the long name or the short name can be used. Long options are set using either of these formats: `--long_name` or `--long_name=value`. Short options are set using either of these formats: `-s` or `-svalue`. If no value is given, then the setting will be set to the closest analogue of true. Booleans are set to `true`. Integers are set to `1`. Floats are set to `1.0`. Strings are set to `""`.

## REPL

To start the REPL, run the game with the `-r` or `--repl` flag.

Print a setting: `\name`  
Set a setting: `\name value`  
Run a command (a callback): `!name`  
Evaluate a duck-lisp form: `(…)`  

Examples:

```
\log_level
\log_level 4
!help
(print "Hello\n")
```

## autoexec.dl

This is a duck-lisp script for power users to modify settings or perform other configuration tasks.

## config.dl

This file is machine generated and may be overwritten by the game. It is preferable for users to edit "autoexec.dl".
