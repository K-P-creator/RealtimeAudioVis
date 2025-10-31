//Main loop for visualization

#include "../include/AudioManager.h"

#include "ImGui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

using namespace std;

static GLFWwindow* createWindow(int,int);
static void toggleFullscreen(GLFWwindow*, bool);
static void error_callback(int, const char*);
static void processInput(GLFWwindow *);
static GLuint recompileShaders();

static GLfloat barColor[4] = { 0.0f, 0.0f, 1.0f, 0.0f };
static GLfloat backgroundColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };


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


    glUseProgram(am.getDefaultShader());
    GLuint colorLocation1 = glGetUniformLocation(am.getDefaultShader(), "BaseColor");
    glUniform4f(colorLocation1, barColor[0], barColor[1], barColor[2], barColor[3]);


    glUseProgram(am.getSymmetricShader());
    GLuint colorLocation2 = glGetUniformLocation(am.getSymmetricShader(), "BaseColor");
    GLuint barCountUniform = glGetUniformLocation(am.getSymmetricShader(), "BarCount");
    glUniform4f(colorLocation2, barColor[0], barColor[1], barColor[2], barColor[3]);
    glUniform1i(barCountUniform, BAR_COUNT);


    bool fullscreen = false;
    bool first = true;

    //Main Loop
    while (!glfwWindowShouldClose(w)) {
        processInput(w);

        glfwGetFramebufferSize(w, &am.settings.windowWidth, &am.settings.windowHeight);
        glViewport(0, 0, am.settings.windowWidth,am.settings.windowHeight);

        glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], backgroundColor[3]);
        glClear(GL_COLOR_BUFFER_BIT);

        // feed inputs to dear imgui, start new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        am.GetAudio();
        am.RenderAudio(w, VBO, VAO);

        // render GUI
        ImGui::Begin("AudioViz Menu");
        if (ImGui::Button("Toggle Fullscreen")) {
            toggleFullscreen(w, fullscreen);
            fullscreen = !fullscreen;
        }

        ImGui::ColorEdit3("Background Color", backgroundColor);
        if (ImGui::ColorEdit3("Bar Color", barColor)) {
            glUseProgram(am.getDefaultShader());
            glUniform4f(colorLocation1, barColor[0], barColor[1], barColor[2], barColor[3]);
            glUseProgram(am.getSymmetricShader());
            glUniform4f(colorLocation2, barColor[0], barColor[1], barColor[2], barColor[3]);
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


//stubby
GLuint recompileShaders()
{
    return GLuint();
}
