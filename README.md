# Rapid Laptimer [![Build](https://github.com/mFlorianW/rapid/actions/workflows/build.yml/badge.svg)](https://github.com/mFlorianW/rapid/actions/workflows/build.yml) [![Build CI Android](https://github.com/mFlorianW/rapid/actions/workflows/build-android.yml/badge.svg)](https://github.com/mFlorianW/rapid/actions/workflows/build-android.yml)
This repository contains source to create motorcycle laptimer for race tracks action.
Everything has the WIP status and is not ready to use.

## Project Structure

### lib Folder
The lib folder contains the rapid library.
Every subfolder in here contains a submodule of the library.
The public API of these library is placed directly in the sub folder.
Private API when present is located in subfolder private.
Everything in the private just an implemenation details and is indirectly tested through the tests of the public API.
Some libraries have a Qt extensions these are optional and are used by some programs.
Qt extension files are located in the sub folder qt

### programs Folder
This folder contains programs which use functionality of the libraries.
There two types of programs.
The first type of programs are intended to run on an embedded device that can be mounted on motorcycle to log the laptimes and accquire additional log information of the motorcycle.
The second type of programs are for the desktop these programs have the functionality to interact with the embedded device and to analyze the laptimes and log data.

### doc Folder
The doc folder contains the project documentation.
The whole documentation is written with ASCIIDoc.
Look at the README.md in that folder to see how to generate the documentation.

### .github
That folder contains specific files for the github.

## Dependencies
The project needs the dependencies:

### Required
* [KDBindings](https://github.com/KDAB/KDBindings)
* [Sqlite3](https://github.com/sqlite/sqlite)
* [Boost Beast](https://github.com/boostorg/beast)
* [Boost Program Options](https://github.com/boostorg/program_options)
* [nlohmann-json](https://github.com/nlohmann/json)
* [spdlog](https://github.com/gabime/spdlog)
* [comms](https://github.com/commschamp/comms)
* [comms-ublox](https://github.com/commschamp/cc.ublox.commsdsl)

### Optional For Tests
* [catch2](https://github.com/catchorg/Catch2)
* [trompeloeil](https://github.com/rollbear/trompeloeil)

### Optional for Desktop Applications
* [Qt6.5](https://download.qt.io/official_releases/qt/6.5/)

With disabled the Qt dependency the project builds only the laptimer

## Build
There are two configure and build presets for CMake.
Call one the in the project root directory to build the LaptimerCore.

[VCPKG](https://vcpkg.io/en/) is used for the dependency handling, so vcpkg must be installed and configured.
Before calling the presets the environment variable "VCPKG_ROOT" must be set.

Enable VCPKG support with the suffix "-vcpkg" to the preset names.

### Debug
#### Configure
```console
cmake --preset debug
```

### Release
#### Configure
``` console
camke --preset release
```

#### Build
``` console
camke --build --preset release
```

### Test
Run all tests in this repository.
The tests are automatically enabled for debug builds and the preset requires the debug configure preset.
``` console
ctest --preset debug --output-on-failure
```

### Icons
The icons are used from the website [www.svgrepo.com](https://github.com/user/repo/blob/branch/other_file.md) and these are licensed under the CC-BY license.
I'm very thankful that I can use them.
