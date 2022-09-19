# Hidey-Chess

Normal chess over the network with camouflaged pieces.

## Dependencies

```text
    SDL2
    SDL2_image
    duck-lisp
        DuckLib
```

## Building

```bash
git submodule update --init --recursive
mkdir build && cd build
cmake ..
cmake --build .
```

## Configuration

Options can be given on the command line. A list of game settings can be found in `docs/settings.md`. Either the long name or the short name can be used. Long options are set using either of these formats: `--long_name` or `--long_name=value`. Short options are set using either of these formats: `-s` or `-svalue`. If no value is given, then the setting will be set to the closest analogue of true. Booleans are set to `true`. Integers are set to `1`. Floats are set to `1.0`. Strings are set to `""`.

## Documentation…

…is in the docs directory.

## Troubleshooting

### Windows

#### Problem

Double-clicking on the executable causes a window to pop up with the following error message.

```text
The prodedure entry point inflateReset2 could not be located in the dynamic link library C:\msys64\mingw64\bin\libpng16-16.dll.
```

#### Solution

Copy msys64's versions of libpng16-16.dll and zlib1.dll into the executable's directory.

### Linux
