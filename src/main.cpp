//Main loop for visualization
#include <iostream>
#include <fstream>
#include <sstream>

#include "../include/AudioManager.h"

#include "ImGui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

using namespace std;

static GLFWwindow* createWindow(int,int);
static void error_callback(int, const char*);
static GLuint openGLInit(GLuint &VBO, GLuint &VAO, GLuint &EBO, GLuint &TBO);
static void processInput(GLFWwindow *);
static string fileToString(const char *);

static GLfloat barColor[4] = { 1.0f, 0.0f, 0.0f, 0.0f };
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
    GLuint VBO, VAO, EBO, TBO;
    auto shaderProgram = openGLInit(VBO, VAO, EBO, TBO);
    glUseProgram(shaderProgram);    //  Once we implement diff shaders, call this inside main loop

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
        am.RenderAudio(w, VBO, TBO);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, BAR_COUNT * 6, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);

        // render GUI
        ImGui::Begin("AudioViz Menu");
        ImGui::ColorEdit3("Background Color", backgroundColor);
        ImGui::ColorEdit3("Bar Color", barColor);
        
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

        ImGui::SliderFloat("Bar Height", &am.settings.barHeightScale, 0.0f, 2.0f);

        ImGui::Checkbox("Smoothing", &am.settings.smoothing);
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

    GLFWwindow* window = glfwCreateWindow(w, h, "OpenGLProject", NULL, NULL);
    if (!window) {
        cerr << "Failed to create window\n";
        std::abort();
    }
    glfwMakeContextCurrent(window);

    return window;
}

static void error_callback(int error, const char* description) {
    cerr << "GLFW Error [" << error << "]: " << description << "\n";
}

static GLuint openGLInit(GLuint& VBO, GLuint& VAO, GLuint& EBO, GLuint &TBO){
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cerr << "Failed to load openGL func pointers with glad\n";
        std::abort();
    }
    cout << "OpenGL Info\n\n";
    cout << "Vendor:   \t" << glGetString(GL_VENDOR) << "\n";
    cout << "Renderer: \t" << glGetString(GL_RENDERER) << "\n";
    cout << "Version:  \t" << glGetString(GL_VERSION) << "\n";
    glfwSwapInterval(1); // Enable vsync


    //  Set up shaders
    int success;
    auto shaderInit = [&success](const char* source, GLuint& name, GLenum type) {
        name = glCreateShader(type);
        glShaderSource(name, 1, &source, NULL);
        glCompileShader(name);
        glGetShaderiv(name, GL_COMPILE_STATUS, &success);

        if (!success) {
            GLint len = 0; glGetShaderiv(name, GL_INFO_LOG_LENGTH, &len);
            std::string log(len, '\0');
            glGetShaderInfoLog(name, len, nullptr, log.data());
            std::cerr << "Shader compile failed (" << name << "):\n" << log << "\n";
            std::abort();
        }
    };

    GLuint vertexShader;
    shaderInit(fileToString("../shaders/basic.vert").data(), vertexShader, GL_VERTEX_SHADER);
    GLuint fragmentShader;
    shaderInit(fileToString("../shaders/basic.frag").data(), fragmentShader, GL_FRAGMENT_SHADER);

    GLuint shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        cerr << "Failed to link shaders\n";
        std::abort();
    }

    glUseProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);


    //  Set up buffers 
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &TBO);


    //  Vertex Array Object 
    glBindVertexArray(VAO);


    //  Vertex Buffer Object
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, BAR_COUNT * sizeof(float) * 4 * 3, nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


    //  Element Buffer Object setup
    std::vector<uint32_t> indices;
    indices.reserve(BAR_COUNT * 6);
    for (uint32_t i = 0; i < BAR_COUNT; ++i) {
        uint32_t base = i * 4;
        // two triangles: 0-1-2, 1-2-3 (CCW)
        indices.push_back(base + 0);
        indices.push_back(base + 1);
        indices.push_back(base + 2);

        indices.push_back(base + 1);
        indices.push_back(base + 2);
        indices.push_back(base + 3);
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);


    //  Texture Buffer Object setup
    glBindBuffer(GL_TEXTURE_BUFFER, TBO);

    return shaderProgram;
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

static string fileToString(const char* path) {
    ifstream file;
    file.open(path);

    if (!file) throw invalid_argument(string("Unable to open file: ") + path);

    ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}