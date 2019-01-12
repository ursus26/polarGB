#include <stdlib.h>
#include <iostream>
#include <vector>
#include <boost/program_options.hpp>
#include "emulator.h"
#include "log.h"


using namespace std;


void printHelp()
{
    cout << "Usage: polarGB [OPTION] [FILE]\n"
         << "Emulates the Game Boy to play FILE.\n"
         << "\n"
         << "Options:\n"
         << "  -h, --help       display this help and exit\n"
         << "  --input-file     input gameboy rom file\n"
         << "  -v, --verbose    verbose printing"
         << endl;

    exit(EXIT_SUCCESS);
}


int main(int argc, char* argv[])
{
    string cartridgePath = "./rom/DMG_ROM.bin";

    /* Parse command line arguments and options. */
    try
    {
        namespace po = boost::program_options;
        po::options_description description("Allowed options");
        description.add_options()
            ("help,h", "Display this help message")
            ("input-file", po::value<vector<string>>(), "Input gameboy rom file")
            ("verbose,v", "Verbose printing");

        po::positional_options_description p;
        p.add("input-file", -1);

        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv).options(description).positional(p).run(),
                  vm);
        po::notify(vm);

        /* Print the program description and quit. */
        if(vm.count("help"))
        {
            printHelp();
        }

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
        cout << e.what() << "\nTry 'gamecub --help' for more information." << endl;
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
