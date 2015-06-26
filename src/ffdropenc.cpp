//
// Created by Seth Parker on 6/15/15.
//

#include "ffdropenc.h"

int main( int argc, char* argv[] ) {

    ////// Read the presets directories //////
    std::vector<ffdropenc::Preset*> preset_list;

    boost::filesystem::path presets_dir(FF_PRESET_DIR);
    ffdropenc::loadPresets( presets_dir, preset_list );

    // Make sure we've found presets
    if( preset_list.size() ) {
        std::cerr << "Presets loaded: " << preset_list.size() << std::endl;
    } else {
        std::cerr << "Error: No presets loaded!" << std::endl;
        return EXIT_FAILURE;
    }

    ////// Parse the command line //////
    int selectedPreset;
    std::vector<std::string> parsedFiles;
    std::string selectedFPS = "30"; // To-Do: Add this to cmd line options

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

    ////// Expand dirs and remove files that don't exist //////
    std::vector<std::string> filteredFileVector;
    std::cerr << "Expanding directories..." << std::endl;
    ffdropenc::ExpandDirs(parsedFiles, filteredFileVector);

    ////// Make our Initial Video Queue //////
    std::cerr << "Making queue..." << std::endl;
    std::vector<ffdropenc::Video> queue;

    std::vector<std::string>::iterator filesIterator = filteredFileVector.begin();
    while ( filesIterator != filteredFileVector.end() ) {
        // Skip this file if it doesn't pass our approved extensions list
        if (!(ffdropenc::isVideo( *filesIterator ) || ffdropenc::isImgSequence( *filesIterator ) )) {
            ++filesIterator;
            continue;
        }

        // Make a new video for this file
        ffdropenc::Video newVideo( *filesIterator, preset_list[selectedPreset] );

        // If Video is an ImgSeq, do the appropriate bookkeeping
        if( ffdropenc::isImgSequence( newVideo.inputPath() ) ) newVideo.convertToSeq(selectedFPS);

        // Add this video to the queue
        queue.push_back( newVideo );

        ++filesIterator;
    }

    ////// Filter the queue of duplicates ////// To-Do

    for ( std::vector<ffdropenc::Video>::iterator queueItem = queue.begin(); queueItem != queue.end(); ++queueItem )
        queueItem->transcode();

    return EXIT_SUCCESS;
}