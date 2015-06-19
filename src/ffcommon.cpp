//
// Created by Seth Parker on 6/19/15.
//

#include "ffcommon.h"

namespace ffdropenc {

    int loadPresets( boost::filesystem::path presetDir, std::vector<ffdropenc::Preset*>& presetsList ) {

        if (boost::filesystem::exists(presetDir)) {

            boost::filesystem::recursive_directory_iterator presetFileIterator(presetDir);
            boost::filesystem::recursive_directory_iterator presetIterator_end; // default construction defaults to iterator.end() value somehow;

            while (presetFileIterator != presetIterator_end) {
                //To-do: Check for .preset extension
                Preset* newFilePreset = new Preset(*presetFileIterator); // Configs don't have copy constructors, so these must be pointers
                presetsList.push_back( newFilePreset );

                ++presetFileIterator;
            }

        }

        return EXIT_SUCCESS;
    }

} // namespace ffdropenc