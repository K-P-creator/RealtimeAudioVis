//Main loop for visualization

#include "../include/AudioManager.h"

#include "ImGui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <chrono>   //  For running bench timer

using namespace std;

static GLFWwindow* createWindow(int, int);
static void toggleFullscreen(GLFWwindow*, bool);
static void error_callback(int, const char*);
static void processInput(GLFWwindow*);


int main() {
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
            std::abort();
        }
    }

    // Create window
    auto w = createWindow(am.settings.windowWidth, am.settings.windowHeight);

    //  Set up openGL
    GLuint VBO, VAO;
    am.openGLInit(VBO, VAO);

    ////////// Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ////////// Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(w, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    ////////// Setup Dear ImGui style
    ImGui::StyleColorsDark();

    const char* modes[] = { "Default", "Symmetric", "Double Symetric" };
    int SmoothingAmt = 1;


    bool fullscreen = false;
    bool first = true;


    //  Benchmarking vars
    std::vector<std::chrono::duration<float>> renderTimes;
    renderTimes.reserve(1000);
    int iterations = 0;
    am.settings.smoothing = false;


    //Main Loop
    while (!glfwWindowShouldClose(w)) {
        processInput(w);

        glfwGetFramebufferSize(w, &am.settings.windowWidth, &am.settings.windowHeight);
        glViewport(0, 0, am.settings.windowWidth, am.settings.windowHeight);

        glClearColor(am.settings.baseColor[0], am.settings.baseColor[1], am.settings.baseColor[2], am.settings.baseColor[3]);
        glClear(GL_COLOR_BUFFER_BIT);

        // feed inputs to dear imgui, start new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        auto st = std::chrono::high_resolution_clock::now();
        am.RenderAudio(w, VBO, VAO);
        auto en = std::chrono::high_resolution_clock::now();

        auto frameDuration = en - st;
        //cout << "Frame Duration:\t" << frameDuration << endl;
        if (iterations > 100 && iterations < 10100)
            renderTimes.push_back(frameDuration);

        else if (iterations > 10100) {
            std::chrono::duration<float> sum(0);
            for (const auto i : renderTimes) {
                sum += i;
            }
            sum /= renderTimes.size();
            cout << "Average per frame render time for 10,000 frames with mode";



            if (am.settings.modeIndex == DEFAULT_M)
            {
                cout << " default";
                am.settings.modeIndex = SYMMETRIC_M;
            }
            else if (am.settings.modeIndex == SYMMETRIC_M)
            {
                cout << " symmetric";
                am.settings.modeIndex = DOUBLE_SYM_M;
            }
            else if (am.settings.modeIndex == DOUBLE_SYM_M && !am.settings.smoothing)
            {
                cout << " double symmetric";
                am.settings.smoothing = true;
            }
            else if (am.settings.smoothing)
            {
                cout << " double sym. with smoothing";
                cout << ":\t" << sum << endl;
                return EXIT_SUCCESS;
            }

            cout << ":\t" << sum << endl;
            cout << "Switching modes..." << endl;

            iterations = 0;
            renderTimes.clear();
        }
        iterations++;

        // render GUI
        ImGui::Begin("AudioViz Menu");
        if (ImGui::Button("Toggle Fullscreen")) {
            toggleFullscreen(w, fullscreen);
            fullscreen = !fullscreen;
        }

        ImGui::ColorEdit3("Background Color", am.settings.baseColor);
        if (ImGui::ColorEdit3("Bar Color", am.settings.barColor)) {
            glUseProgram(am.getDefaultShader());
            glUniform4f(am.getColorLocation1(), am.settings.barColor[0], am.settings.barColor[1], am.settings.barColor[2], am.settings.barColor[3]);
            glUseProgram(am.getSymmetricShader());
            glUniform4f(am.getColorLocation2(), am.settings.barColor[0], am.settings.barColor[1], am.settings.barColor[2], am.settings.barColor[3]);
        }

        if (ImGui::BeginCombo("Mode", modes[am.settings.modeIndex])) {
            for (int i = 0; i < IM_ARRAYSIZE(modes); i++) {
                bool is_selected = (am.settings.modeIndex == i);
                if (ImGui::Selectable(modes[i], is_selected))
                    am.settings.modeIndex = i;
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }

            ImGui::EndCombo();
        }

        ImGui::Checkbox("Smoothing", &am.settings.smoothing);
        if (am.settings.smoothing) {
            if (ImGui::SliderInt("Smoothing Amount", &SmoothingAmt, 1, 5)) {
                am.UpdateSmoothing(SmoothingAmt);
            }
        }

        ImGui::SliderFloat("Bar Height", &am.settings.barHeightScale, 0.0f, 2.0f);

        ImGui::End();

        // Render dear imgui into screen
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        glfwPollEvents();
        glfwSwapBuffers(w);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();


    return EXIT_SUCCESS;
}


static GLFWwindow* createWindow(int w, int h)
{
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
        cerr << "Failed to initialize glfw\n";
        std::abort();
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window;
    window = glfwCreateWindow(w, h, "AudioVis", NULL, NULL);


    if (!window) {
        cerr << "Failed to create window\n";
        std::abort();
    }
    glfwMakeContextCurrent(window);

    return window;
}


static void toggleFullscreen(GLFWwindow* window, bool fullScrn) {
    const auto mon = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(mon);

    // saved window states
    static int x, y, hi, wi;

    if (fullScrn)
    {
        glfwSetWindowMonitor(window, NULL, x, y, wi, hi, 0);
    }
    else
    {
        glfwGetWindowSize(window, &wi, &hi);
        glfwGetWindowPos(window, &x, &y);
        glfwSetWindowMonitor(window, mon, 0, 0, mode->width, mode->height, mode->refreshRate);
    }
}


static void error_callback(int error, const char* description) {
    cerr << "GLFW Error [" << error << "]: " << description << "\n";
}


void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

