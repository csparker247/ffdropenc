//
// Created by Seth Parker on 6/15/15.
//

#define FFDROPENC_PRESET_DIR "presets/" // For development only

#include "ffdropenc.h"

int main( int argc, char* argv[] ) {

    ////// Read the presets directories //////
    std::vector<ffdropenc::Preset*> preset_list;

    boost::filesystem::path presets_dir(FFDROPENC_PRESET_DIR);
    ffdropenc::loadPresets( presets_dir, preset_list );

    // Make sure we've found presets
    if( preset_list.size() ) {
        std::cout << preset_list.size() << " presets loaded." << std::endl;
    } else {
        std::cerr << "Error: No presets loaded!" << std::endl;
        return EXIT_FAILURE;
    }

    ////// Parse the command line //////
    int selectedPreset;
    std::vector<std::string> parsedFiles;

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
            selectedPreset = parsedOptions["preset"].as<int>();
        } else {
            std::cerr << "ERROR: Preset not supplied!" << std::endl;
            return EXIT_FAILURE;
        }

        // Transfer the parsed strings into the parsedFiles vector for easier access
        if (parsedOptions.count("input-file")) {
            parsedFiles = parsedOptions["input-file"].as<std::vector<std::string> >();
        } else {
            std::cerr << "ERROR: Input file(s) not supplied!" << std::endl;
            return EXIT_FAILURE;
        }
    }
    catch(std::exception& e)
    {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    ////// To-Do: Handle Directories in parsedFiles //////

    ////// Make our Video Queue //////
    std::vector<ffdropenc::Video> queue;

    std::vector<std::string>::iterator filesIterator = parsedFiles.begin();
    while (filesIterator != parsedFiles.end()) {
        // To-Do: check that these files actually exist
        ffdropenc::Video newVideo( *filesIterator, preset_list[selectedPreset] );
        queue.push_back( newVideo );

        ++filesIterator;
    }

    queue[0].transcode();


    return EXIT_SUCCESS;
}