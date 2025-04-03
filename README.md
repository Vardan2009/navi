## NAVI - Command-line file picker

![navi-demo](https://github.com/user-attachments/assets/f3754125-5456-4ed6-a47c-27256cc4042b)

NAVI is a very simple, but useful file picker written in C using the NCurses library for visuals.

## Features
- File Traversal (Entering/Leaving directories)
- Basic Searching

## Usage
```
navi [path] [-f] [-d] [-m "Message"]
```
- `path`, The initial path (default value is the PWD of the terminal) 
- `-f`, Only allow selecting a file
- `-d`, Only allow selecting a directory
- `-m`, Add an additional message

## Building
### 1. Install the NCurses library
*Example with apt*
```
$ sudo apt-get install libncurses-dev
```
### 2. Locate `libncursesw.so`
*I couldn't get CMake to automatically get the library*
```
$ locate libncursesw.so
/path/to/libncursesw.so
```
### 3. Change the path to `libncursesw.so` in `/CMakeLists.txt`
### 4. Use CMake to build
```
$ mkdir build
$ cd build
$ cmake ..
$ make
```
