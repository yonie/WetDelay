# VST3 WET Delay Plugin - Project Documentation

## Project Overview

A VST3 stereo delay plugin with the following features:

- **Delay Time Control**: 6-position tip switch (20ms, 40ms, 80ms, 120ms, 220ms, 400ms)
- **Input LR Meters**: Real-time visual level meters for Left and Right input channels
- **Output LR Meters**: Real-time visual level meters for Left and Right output channels
- **100% Wet Output**: Plugin always outputs pure delayed signal (no dry signal mixed in)

## Approach

As these kind of projects can get out of hand real fast, we should keep it simple and focused. We should prioritize core functionality and avoid unnecessary complexity.

## Build System

We are in Windows, Visual Studio Code, using Kilo Code AI coding assistant. 
We have Visual Studio Build Tools 2022 (MSVC compiler)

## Development Environment

- **IDE**: Visual Studio 2022 Community Edition
- **Code Formatting**: Clang-Format with Steinberg's VST3 SDK style
- **Version Control**: Git with GitHub repository
- **Testing**: Steinberg's VST3 Validator and custom test suite
- **Documentation**: Markdown files for project documentation and README

Author Ronald Klarenbeek yonie@yonie.org https://github.com/yonie/