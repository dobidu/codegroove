# Codegroove
CodeGroove is an open-source terminal application used to create and manipulate audio using live coding techniques. 

## Requirements 

This project runs only on Linux and uses [Popsicle](https://github.com/juce-framework/JUCE/tree/develop?tab=readme-ov-file#building-examples), which is included with [JUCE](https://github.com/juce-framework/JUCE/tree/develop?tab=readme-ov-file#building-examples). It also requires Python 3.10 or higher.

Building it requires CMake version 3.22 or higher and the [Linux dependencies of JUCE](https://github.com/juce-framework/JUCE/blob/develop/docs/Linux%20Dependencies.md).

## Build

First, clone the repository recursively.

```bash
git clone --recursive https://github.com/dobidu/codegroove
```

After that, go inside the project's folder and run the following commands:

```bash
cmake -B build
cmake --build build
```

The application should be located inside *./build/plugin/CodeGroove_artefacts/Release*.