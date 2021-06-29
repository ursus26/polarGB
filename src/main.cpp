/**
 * Copyright (C) 2018 Bart de Haan
 *
 * polarGB is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * polarGB is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <memory>
#include <stdlib.h>
#include <vector>
#include <boost/program_options.hpp>
#include <fmt/format.h>
#include "polarGB/emulator.h"


using namespace std;


const size_t MAJOR_VERSION = 0;
const size_t MINOR_VERSION = 0;


void printHelp()
{
    fmt::print("Usage: polarGB [OPTION] [FILE]\n");
    fmt::print("Emulates the Game Boy to play FILE.\n\n");
    fmt::print("Options:\n");
    fmt::print("  -h, --help           Display this help information\n");
    fmt::print("      --input-file     Input gameboy rom file\n");
    fmt::print("      --version        Display emulator version information\n");

    exit(EXIT_SUCCESS);
}


void printVersion()
{
    fmt::print("polarGB version {}.{}\n", MAJOR_VERSION, MINOR_VERSION);
    exit(EXIT_SUCCESS);
}


int main(int argc, char* argv[])
{
    string cartridgePath = "./rom/tetris.gb";

    /* Parse command line arguments and options. */
    try
    {
        namespace po = boost::program_options;
        po::options_description description("Allowed options");
        description.add_options()
            ("help,h", "Display this help information")
            ("input-file", po::value<vector<string>>(), "Input gameboy rom file")
            ("version", "Display emulator version information");

        po::positional_options_description p;
        p.add("input-file", -1);

        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv).options(description).positional(p).run(),
                  vm);
        po::notify(vm);

        if(vm.count("help"))
            printHelp();
        else if(vm.count("version"))
            printVersion();

        /* Get the input rom file. */
        if(vm.count("input-file"))
        {
            vector<string> files = vm["input-file"].as<vector<string>>();
            cartridgePath = files.front();
        }
    }
    catch(exception& e)
    {
        fmt::print(stderr, "Error, {}\nTry 'polarGB --help' for more information.\n", e.what());
        return EXIT_FAILURE;
    }

    /* Start the emulator and load the cartridge. */
    unique_ptr<Emulator> emu = make_unique<Emulator>();
    emu->start(cartridgePath);

    return EXIT_SUCCESS;
}
