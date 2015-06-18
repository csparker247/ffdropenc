//
// Created by Seth Parker on 6/15/15.
//

#include "ffdropenc.h"

int main( int argc, char* argv[] ) {

    // Variables we need to keep from the command line
    int selectedPreset;
    std::vector<std::string> parsedFiles;

    // Try so that we can catch boost/program_options errors
    try {
        // All command line options
        boost::program_options::options_description options("Options");
        options.add_options()
                ("help,h", "Show this message")
                ("input-file,i", boost::program_options::value<std::vector<std::string> >(),
                 "File(s) you want transcoded")
                ("preset,p", boost::program_options::value<int>(&selectedPreset)->default_value(0),
                 "Index of preset to use");

        // Map position options to the input-file vector
        boost::program_options::positional_options_description positional_options;
        positional_options.add("input-file", -1);

        // parsedOptions will hold the values of all parsed options as a Map
        boost::program_options::variables_map parsedOptions;
        boost::program_options::store(
                boost::program_options::command_line_parser(argc, argv).options(options).positional(
                        positional_options).run(), parsedOptions);
        boost::program_options::notify(parsedOptions);

        // Show the help message
        if (parsedOptions.count("help") || argc < 2) {
            std::cout << options << std::endl;
            return EXIT_SUCCESS;
        }

        if (parsedOptions.count("preset")) {
            std::cout << parsedOptions["preset"].as<int>() << std::endl;
        }

        // Transfer the parsed strings into the parsedFiles vector for easier access
        if (parsedOptions.count("input-file")) {
            parsedFiles = parsedOptions["input-file"].as<std::vector<std::string> >();
            std::vector<std::string>::iterator filesIterator = parsedFiles.begin();
            while (filesIterator != parsedFiles.end()) {
                std::cout << *filesIterator << std::endl;
                ++filesIterator;
            }
        }
    }
    catch(std::exception& e)
    {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}