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

#include <stdlib.h>
#include <iostream>
#include <vector>
#include <boost/program_options.hpp>
#include "polarGB/emulator.h"
#include "polarGB/log.h"


using namespace std;


const size_t MAJOR_VERSION = 0;
const size_t MINOR_VERSION = 0;


void printHelp()
{
    cout << "Usage: polarGB [OPTION] [FILE]\n"
         << "Emulates the Game Boy to play FILE.\n"
         << "\n"
         << "Options:\n"
         << "  -h, --help           Display this help information\n"
         << "      --input-file     Input gameboy rom file\n"
         << "  -v, --verbose        Verbose printing\n"
         << "      --version        Display emulator version information"
         << endl;

    exit(EXIT_SUCCESS);
}


void printVersion()
{
    cout << "polarGB version " << MAJOR_VERSION << "." << MINOR_VERSION
         << endl;

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
            ("verbose,v", "Verbose printing")
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

        /* Set the verbose flag. */
        if(vm.count("verbose"))
        {
            Log::setVerbose(true);
        }
        else
        {
            Log::setVerbose(false);
        }
    }
    catch(exception& e)
    {
        cout << e.what() << "\nTry 'polarGB --help' for more information." << endl;
        return EXIT_FAILURE;
    }

    /* Start the emulator and load the cartridge. */
    Emulator* emu = new Emulator();
    emu->loadCartridge(cartridgePath);
    emu->run();

    /* Clean up. */
    delete emu;
    emu = nullptr;

    return EXIT_SUCCESS;
}
