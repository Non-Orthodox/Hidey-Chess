# Settings: for developers

This is based off of GNU's command line options, but it has been written from scratch.

To add your own, you only have to update `SETTINGS_LIST` in `main.cpp`. `SETTINGS_ALIAS_LIST` is only if you want to refer to it using a single letter as well. Don't use spaces in the names of settings if you want to be able to set it on the command line.

Be aware of the type that is assigned to a setting when you create it. The type is inferred from the type of the default value that you give it. There are four types that can be inferred: `bool`, `int`, `float`, and `std::string`.

Settings can have callbacks assigned to them. These are run when a variable is set. This is mainly used for input sanitization and updating global variables.

Fun fact: All arguments are checked for options, including `argv[0]`, which is the program name. `argv[0]` will contain ./Hidey-Chess on Linux, but you could put it in a directory like `/usr/local/bin`, which will allow you to execute it by running just `Hidey-Chess`. It can then be renamed to something like `-aexample.com`, and the program should interpret it's own name as an argument. Weird, but I don't think it hurts anything, so I'm thinking I'll just leave it since it's kind of fun.
