//Main loop for visualization
#include <iostream>

#include "../include/AudioManager.h"

using namespace std;

int main(){
    // Create window

    

    // Create the audio manager
    AudioManager am;


    // Make sure am is valid when initializing
    unsigned int trys = 0;
    try {
        am = {};
    }
    catch (const runtime_error& e) {
        cerr << "Error initialing audio manager... Retrying..." << endl;
        
        
        am = {};
        trys++;

        if (trys >= RETRY_COUNT) {
            cerr << "Reinitialize failed " << trys << " times... Exiting...\n";
            return EXIT_FAILURE;
        }
    }

    trys = 0;


    //Main Loop


    return EXIT_SUCCESS;
}