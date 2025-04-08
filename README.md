## NAVI - Command-line file picker

![navi-demo](/artwork/navi-demo.png)

NAVI is a very simple, but useful file picker written in C using the NCurses library for visuals.\
It works well with shell scripts, makefiles, e.t.c, since it just outputs the path after selecting it.

> [!WARNING]  
> NAVI is not tested on Windows or MacOS yet.

#### Example usage in bash
```bash
path=$(navi ~ -m "Select a file")
./some/command $path
```

## Features
- File Traversal (Entering/Leaving directories)
- Basic Searching
- Customization with configuration files 

## Usage
```bash
navi [path] [-f] [-d] [-m "Message"]
```
- `path`, The initial path (default value is the PWD of the terminal) 
- `-f`, Only allow selecting a file
- `-d`, Only allow selecting a directory
- `-m`, Add an additional message

## `navi.cfg` file
> [!WARNING]  
> NAVI is not tested on Windows or MacOS yet.

The program attempts to load an optional configuration file stored in application data directories. If you want to customize navi, create a file at that path.
- Windows: `%USERPROFILE%\\AppData\\Local\\navi.cfg`
- Linux: `~/.config/navi.cfg`
- Mac: `~/Library/Application Support/navi.cfg`\
_Refer to [config.h](/include/config.h)_

## Building
### 1. Install the NCurses library
*Example with apt*
```bash
sudo apt-get install libncurses-dev
```
**Make sure to also install NCursesW for Unicode character support**

### 2. Use CMake to build
```bash
mkdir build
cd build
cmake ..
make
```
*or use the `/test.sh` file*
