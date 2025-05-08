//Main loop for visualization
#include <iostream>
#include <optional>

#include "../include/AudioManager.h"

using namespace std;

int main(){
    // Create window
    sf::RenderWindow window(sf::VideoMode({ WINDOW_WIDTH, WINDOW_HEIGHT }), "AudioVis");
    bool fullScreen = false;


    // Init clock for checking when to run next frame
    sf::Clock clock;

    // Clock for keys pressed to prevent switching loop
    sf::Clock keyClock;
    keyClock.start();
    

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

    char graphType = 'd'; // Used to select which audio display function we use. switch modes with M key

    //Main Loop
    while (window.isOpen())
    {
        sf::Time keyTime = keyClock.getElapsedTime();

        // Keep track of keys being pressed
        bool esc = false;
        bool m = false;
		bool c = false;

        while (const optional<sf::Event> event = window.pollEvent()) 
        {
            //Handle events
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                if (key->scancode == sf::Keyboard::Scancode::Escape && keyTime >= chrono::seconds(5)) {
                    esc = true;
                }
                else if (key->scancode == sf::Keyboard::Scancode::M) {
                    m = true;
				}
                else if (key->scancode == sf::Keyboard::Scancode::C) {
                    c = true;
                }
            }

            if (const auto* key = event->getIf<sf::Event::KeyReleased>()) {
                if (key->scancode == sf::Keyboard::Scancode::Escape) {
                    esc = false;
                }
                else if (key->scancode == sf::Keyboard::Scancode::Escape) {
                    m = false;
                }
                else if (key->scancode == sf::Keyboard::Scancode::C) {
                    c = false;
                }
            }




            if (esc && keyTime >= chrono::seconds(3)) {
                if (fullScreen) {
                    cout << "Switching to Windowed mode\n";
                    window.close();
                    window.create(sf::VideoMode({ WINDOW_WIDTH, WINDOW_HEIGHT }), "AudioVis");
                    fullScreen = false;
                }
                else if (!fullScreen) {
                    cout << "Switching to FullScreen mode\n";
                    window.close();
                    window.create(sf::VideoMode({ WINDOW_WIDTH, WINDOW_HEIGHT }), "AudioVis", sf::State::Fullscreen);
                    fullScreen = true;
                }
                keyClock.restart();
                keyTime = chrono::seconds(0);
            }

            if (m && keyTime >= chrono::milliseconds(100)) {
                if (graphType == 'd')
                {
                    cout << "Switching to symmetric graph\n";
                    graphType = 's';
                }
                else if (graphType == 's')
                {
                    cout << "Switching to double symmetric graph\n";
                    graphType = 'f';
                }
                else if (graphType == 'f') {
                    cout << "Switching to double sym with smoothing\n";
                    graphType = 'n';
                }
				else if (graphType == 'n') {
					cout << "Switching to curve\n";
					graphType = 'c';
				}
                else 
                {
                    cout << "Switching to default graph\n";
                    graphType = 'd';
                }

                keyClock.restart();
                keyTime = chrono::seconds(0);
            }

			if (c && keyTime >= chrono::milliseconds(100)) {
				cout << "Switching to color function\n";
				am.SetColorFunction();
				keyClock.restart();
				keyTime = chrono::seconds(0);
			}
        }

        //Check clock and if enough time has elapsed run another frame 
        if (clock.getElapsedTime() >= FRAME_TIME) {
            clock.restart();


            //Gather Audio Information
            try {
                am.GetAudio();
            }
            catch (const runtime_error& e){
                std::cerr << "Audio error: " << e.what() << ". Attempting to reinitialize..." << std::endl;
                //If GetAudio failed due to change in audio source, reinitialize and try again
                am = {};
                trys++;

                if (trys >= RETRY_COUNT) {
                    cerr << "Reinitialize failed " << trys << " times... Exiting...\n";
                    return EXIT_FAILURE;
                }
            }

            //Draw to screens
            window.clear();

            switch (graphType) {
            case 'd':
                am.RenderAudio(&window);
                break;
            case 's':
                am.RenderAudioSymmetric(&window);
                break;
            case 'f':
                am.RenderAudioFourWaySym(&window);
                break;
            case 'n':
                am.RenderAudioWithSmoothing(&window);
				break;
			case 'c':
				am.RenderAudioCurve(&window);
				break;
            }

            window.display();
        }
    }

    return EXIT_SUCCESS;
}