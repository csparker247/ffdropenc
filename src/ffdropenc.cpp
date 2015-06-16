//
// Created by Seth Parker on 6/15/15.
//

#include "ffdropenc.h"

int main( int argc, char* argv[] ) {

    boost::program_options::options_description options("Options");
    options.add_options()
            ("input-file,i", "File you want transcoded")
            ("preset,p", "Preset to use")
    ;

    return EXIT_SUCCESS;
}