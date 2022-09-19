# Scripting

Hidey-Chess features a minimal yet powerful scripting system that is compliant with Greenspun's tenth rule.

## Basic scripting

The language is an extension of the settings system using callbacks, so it retains many of the ideas from it, but there are a few new features as well.

Every variable and command is a setting, so they can store one of the four types of data, and when it is set a callback function is run if one exists.
Commands always have a callback function, but not all variables do. The only real difference between a command and a variable is how it is used.

Here are some examples.

```lisp
network_port 5
```

Set `network_port` to 5. Pretty simple. Nice for command lines.

```lisp
print network_port
```

Print the value of `network_port`. For config files, this is all you really need to know, but for more complicated scripting it is important to realize that `print`'s value after execution is the string "network_port". To read the value of `print` on the command line, simply run `print print`. While here we see that the argument is the same as the final value of the command, this is not always true. The command can set its value to whatever it likes. An example is `set`.

```lisp
set network_port 5
```

If `set` used a callback that was similar to `print`, the final value of `set` would be "network_port 5". When we get to passing values to other commands, this isn't too useful. Instead, the final value of `set` is "5". Later, this will allow us to chain multiple `set`s together to set multiple variables to the same value. It is necessary to look at the documentation or source code in order to determine what a command's final value is set to. There is currently no standard.

## Advanced scripting

This form of scripting relies upon the command `""`, or in order to easily refer to it, the "chain". The command is an empty string with a length of zero. The chain has three features. The first is that it does absolutely nothing to normal strings. `--=network_port` does nothing. `--=5` also does nothing. The second feature is that commands in parentheses are run before the rest of the command. This is just like algebra (or an even better analogy, Lisp). `"--=(network_port 5)"` sets `network_port` to 5. Quotes were added to force the OS to treat the option as one argument. Since chain does nothing other than run commands in parentheses, we can add multiple commands to the same chain: `"--=(network_port 5) (print network_port)"`. I recommend making chain the first command run for all non-trivial scripts. The third feature is the final command value mentioned in the section above. This final value is returned to the command above in place of the command in parentheses. A useful example of it's use can be seen below.

```lisp
; Set multiple variables to the same value.
(network_port (peer_network_port (network_port 5)))
```

Running the chain command may be difficult to run from from a string, but once it has been called once, it is easy to call again.

```lisp
;; "( ())" runs the chain command, but this has to be in a subcommand
;; of the chain. I'm not entirely sure if there is a good reason to
;; do this. Prints "5".
--=(echo ( 5))
; Syntax error:
--=(echo (5))
```

## Notes

I have never used Lisp, so it is likly that the only similarity is the syntax. I only chose the syntax because it was so easy to make.
C++'s string library helped a lot. I don't think OOP helped at all. I think I would rather have all variables public and also make the functions separate from the data. The `this` keyword was nice though. I'd very much like to have that in C for use in callbacks. Maybe I can make a `#define` for it.
