# Hidey-Chess

Normal chess over the network with camouflaged pieces.

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
