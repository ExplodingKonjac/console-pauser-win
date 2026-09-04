# console-pauser-win

This is a wrapper program on Windows that runs a process and measure its user time, system time and peak memory, then print the information and a `Press any key to continue...` message.

## Installation

If you have MinGW or MSYS, just run:

```bash
make install
```

and it will be installed on `/usr/local`.

Otherwise you should specify environment variable `CXX` and `PREFIX` according to your compiler.

## Usage

Obviously:

```bash
console-pauser <prog> <args>...
```
