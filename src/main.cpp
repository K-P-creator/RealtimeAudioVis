//Main loop for visualization

#include "../include/AudioManager.h"

#include <chrono>

#include <ImGui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

using namespace std;
using FPSclock = chrono::steady_clock;

static GLFWwindow* createWindow(int,int);
static void toggleFullscreen(GLFWwindow*, bool);
static void error_callback(int, const char*);
static void processInput(GLFWwindow *);
static inline int calculateFPS(long long);

struct ImGuiSettings{
    bool perfOverlay;
    bool fullscreen;
};

static ImGuiSettings GuiSettings = {
    true,
    false
};


int main() {
    // Create the audio manager
    AudioManager am;

    // Make sure am is valid when initializing
    try {
        am = {};
    }
    catch (const runtime_error& e) {
        unsigned int trys = 0;
        cerr << "Error initialing audio manager... Retrying..." << endl;
        am = {};
        trys++;
        if (trys >= RETRY_COUNT) {
            cerr << "Reinitialize failed " << trys << " times... Exiting...\n";
            std::abort();
        }
    }

    // Create window
    const auto w = createWindow(am.settings.windowWidth, am.settings.windowHeight);

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

    const char* modes[3] = { "Default", "Symmetric", "Double Symetric" };
    int SmoothingAmt = 1;



    bool first = true;

    //  Performance Metrics
    bool hotFrame = false;
    int fps = 0;
    long long dt_us = 0;
    long long frameNumber = 0;
    auto fpsTimerStart = FPSclock::now();
    auto fpsTimerEnd = FPSclock::now();

    //Main Loop
    while (!glfwWindowShouldClose(w)) {
        //  Every 100 frames re-calculate perf data
        if (frameNumber % 100 == 0)
            hotFrame = true;
        else hotFrame = false;


        if (hotFrame)
            fpsTimerStart = FPSclock::now();

        processInput(w);

        glfwGetFramebufferSize(w, &am.settings.windowWidth, &am.settings.windowHeight);
        glViewport(0, 0, am.settings.windowWidth,am.settings.windowHeight);

        glClearColor(am.settings.baseColor[0], am.settings.baseColor[1], am.settings.baseColor[2], am.settings.baseColor[3]);
        glClear(GL_COLOR_BUFFER_BIT);

        // feed inputs to dear imgui, start new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        am.RenderAudio(w, VBO, VAO);

        // render GUI
        ImGui::Begin("AudioViz Menu");
        if (ImGui::Button("Toggle Fullscreen")) {
            toggleFullscreen(w, GuiSettings.fullscreen);
            GuiSettings.fullscreen = !GuiSettings.fullscreen;
        }

        ImGui::ColorEdit3("Background Color", am.settings.baseColor);
        if (ImGui::ColorEdit3("Bar Color", am.settings.barColor)) {
            const auto state = am.settings.modeIndex;

            glUseProgram(am.getDefaultShader());
            glUniform4f(am.getColorLocation1(), am.settings.barColor[0], am.settings.barColor[1],
                        am.settings.barColor[2], am.settings.barColor[3]);
            glUseProgram(am.getSymmetricShader());
            glUniform4f(am.getColorLocation2(), am.settings.barColor[0], am.settings.barColor[1],
                        am.settings.barColor[2], am.settings.barColor[3]);
            glUseProgram(am.getDoubleSymmetricShader());
            glUniform4f(am.getColorLocation3(), am.settings.barColor[0], am.settings.barColor[1],
                        am.settings.barColor[2], am.settings.barColor[3]);

            switch (state) {
            case SYMMETRIC_M:
                glUseProgram(am.getSymmetricShader());
                break;
            case DOUBLE_SYM_M:
                glUseProgram(am.getDoubleSymmetricShader());
                break;
            default:
                glUseProgram(am.getDefaultShader());
            }
        }
        
        if (ImGui::BeginCombo("Mode", modes[am.settings.modeIndex])) {
            for (int i = 0; i < IM_ARRAYSIZE(modes); i++) {
                const bool is_selected = (am.settings.modeIndex == i);
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

        if (GuiSettings.perfOverlay){
            ImGui::Begin("Performance Overlay");

            ImGui::Text("FPS: %d", fps);
            ImGui::Text("Frame time: %.2f ms", static_cast<double>(dt_us)/1e3);

            ImGui::End();
        }

        // Render dear imgui into screen
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        glfwPollEvents();
        glfwSwapBuffers(w);


        if (hotFrame) {
            fpsTimerEnd = FPSclock::now();
            FPSclock::duration diff = fpsTimerEnd - fpsTimerStart;
            dt_us = std::chrono::duration_cast<std::chrono::microseconds>(diff).count();
            fps = calculateFPS(dt_us);
        }
        frameNumber++;
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();


    return EXIT_SUCCESS;
}

static GLFWwindow* createWindow(const int w, const int h)
{
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
        cerr << "Failed to initialize glfw\n";
        std::abort();
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(w, h, "AudioVis", nullptr, nullptr);


    if (!window) {
        cerr << "Failed to create window\n";
        std::abort();
    }
    glfwMakeContextCurrent(window);

    return window;
}

static void toggleFullscreen(GLFWwindow* window, const bool fullScreen) {
    const auto mon = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(mon);

    // saved window states
    static int x, y, hi, wi;

    if (fullScreen)
    {
        glfwSetWindowMonitor(window, nullptr, x, y, wi, hi, 0);
    }
    else
    {
        glfwGetWindowSize(window, &wi, &hi);
        glfwGetWindowPos(window, &x, &y);
        glfwSetWindowMonitor(window, mon, 0, 0, mode->width, mode->height, mode->refreshRate);
    }
}

static void error_callback(const int error, const char* description) {
    cerr << "GLFW Error [" << error << "]: " << description << "\n";
}


void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}


//  FPS = 1/(frame time(s))
static inline int calculateFPS(const long long dt_us){
    if (dt_us <= 0) return 0;
    return static_cast<int>(1e6 / static_cast<double>(dt_us));
}