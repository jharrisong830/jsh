# jsh - a small custom shell written in C

### Executable Download: [jsh](https://github.com/jharrisong830/jshell/raw/main/jsh)

***

## Setup Guide

1. Download the [executable](https://github.com/jharrisong830/jshell/raw/main/jsh), **or** clone this repository and `make` from the source code
2. Execute from your current working directory (with `./jsh`), or move the executable to a PATH folder to start from anywhere on your system (with `jsh`)

## Features

- **cd**: changes directory to the given path, if it exists. Can take in paths with spaces in filenames, assuming that all "args" after `cd` compose a single valid path
  - `cd` --> changes to the current user's home directory
  - `cd <pathname>` --> changes to the specified `pathname`, throws an error if `pathname` is not valid
- **ls**: basic and colorized listing command that lists all non-hidden files and directories
  - `ls` --> lists the contents of the current working directory
  - `ls <pathname> ...` -> lists the contents of the directory (or directories, if multiple paths are provided) specified by `pathname`
  - **NOTE**: this command currently does not work with paths that have filenames with spaces. Behavior is unexpected
- **exit**: ends the shell session
  - `exit` --> terminates the current shell session
- **Support for other commands**: enabled through the use of `exec()`
- **Handles signals**: responds to `SIGINT` (`Ctrl+C`) in the same manner as other shells (bash, zsh, ...)
- **And more coming soon...!**

***

### Usage Notes

This is a personal project, and therefore is **NOT** recommended for personal or official use. This program should only be used for experimentation. Behavior of any command (even ones invoked using `exec()`) is not guaranteed to be correct. Please see the [license](https://github.com/jharrisong830/jsh/blob/main/LICENSE) for official details.

jsh was written in C for Linux platforms, and will not work on other platforms (such as macOS or Windows). I suggest only running this in a virtual machine running a Linux-based OS.
