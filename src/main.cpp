//Main loop for visualization
#include <iostream>

#include "../include/AudioManager.h"

using namespace std;

static GLFWwindow* createWindow();
static void error_callback(int error, const char* description);
static void openGLInit();

int main(){
    // Create window
    auto w = createWindow();

    //  Set up openGL
    openGLInit();

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

static GLFWwindow* createWindow()
{
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
        cout << "Failed to initialize glfw\n";
        std::abort();
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "OpenGLProject", NULL, NULL);
    if (!window) {
        cout << "Failed to create window\n";
        std::abort();
    }
    glfwMakeContextCurrent(window);

    return window;
}

static void error_callback(int error, const char* description) {
    cerr << "GLFW Error [" << error << "]: " << description << "\n";
}

void openGLInit(){
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cout << "Failed to load openGL func pointers with glad\n";
        std::abort();
    }
    cout << "Vendor:   " << reinterpret_cast<const char*>(glGetString(GL_VENDOR)) << "\n";
    cout << "Renderer: " << glGetString(GL_RENDERER) << "\n";
    cout << "Version:  " << glGetString(GL_VERSION) << "\n";
    glfwSwapInterval(1); // Enable vsync
}
