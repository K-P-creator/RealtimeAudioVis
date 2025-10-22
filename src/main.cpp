//Main loop for visualization
#include <iostream>

#include "../include/AudioManager.h"

#include "ImGui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

using namespace std;

static GLFWwindow* createWindow();
static void error_callback(int, const char*);
static GLuint openGLInit(GLuint &VBO, GLuint &VAO, GLuint &EBO);
static void processInput(GLFWwindow *);

int main(){
    // Create window
    auto w = createWindow();

    //  Set up openGL
    GLuint VBO, VAO, EBO;
    auto shaderProgram = openGLInit(VBO, VAO, EBO);
    glUseProgram(shaderProgram);    //  Once we implement diff shaders, call this inside main loop

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

    ////////// Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ////////// Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(w, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    ////////// Setup Dear ImGui style
    ImGui::StyleColorsDark();

    //Main Loop
    while (!glfwWindowShouldClose(w)) {
        processInput(w);

        int width, height;
        glfwGetFramebufferSize(w, &width, &height);
        glViewport(0, 0, width, height);

        glClearColor(0.0f,1.0f,0.0f,0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // feed inputs to dear imgui, start new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        am.GetAudio();
        am.RenderAudio(w, VBO);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, BAR_COUNT * 6, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);

        // render GUI
        ImGui::Begin("Demo window");
        ImGui::Button("Enable/Disable Smoothing");
        ImGui::Button("Background Color");
        ImGui::Button("Bar Color");
        ImGui::Button("Display Mode");
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

static GLFWwindow* createWindow()
{
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
        cerr << "Failed to initialize glfw\n";
        std::abort();
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "OpenGLProject", NULL, NULL);
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

static const char* vertexShaderSource = "#version 330 core\nlayout (location = 0) in vec3 aPos;\nvoid main()\n{\ngl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n}\0";
static const char* fragmentShaderSource = "#version 330 core\nout vec4 FragColor;\nvoid main()\n{\nFragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n}\0";

GLuint openGLInit(GLuint& VBO, GLuint& VAO, GLuint& EBO){
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
    shaderInit(vertexShaderSource, vertexShader, GL_VERTEX_SHADER);
    GLuint fragmentShader;
    shaderInit(fragmentShaderSource, fragmentShader, GL_FRAGMENT_SHADER);

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

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, BAR_COUNT * sizeof(float) * 4 * 3, nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    //  Set up the EBO
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

    return shaderProgram;
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
