# polarGB game boy emulator

polarGB is a game boy emulator written in C++17. It is currently not finished and I recommend using other emulators.

## Dependencies
* C++17 compiler
* Make
* CMake
* SDL2
* Boost Program Options

## Building
```
git clone https://github.com/ursus26/polarGB
cd polarGB/
git submodule update --init --recursive
CMake .
make
```
All executable binaries are placed in the `./bin` folder.

## Usage
Main usage
```
./bin/polarGB ./path/to/gameboy/game.rom
```

Help
```
./bin/polarGB -h
```

Tests
```
./bin/tests
```

## License
GNU General Public License v3.0. See [LICENSE](LICENSE) for more information.
