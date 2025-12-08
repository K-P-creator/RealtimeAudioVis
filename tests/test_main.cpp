//  We need to set up an instance of GL in order to test certain things such as shader compilation.
//  This will override the main() of GTest in order to do so. 

#include <gtest/gtest.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../include/AudioManager.h"

class GLTestEnvironment : public ::testing::Environment {
public:
    virtual void SetUp() {
        ASSERT_TRUE(glfwInit());

        // Create hidden window for GL context
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        window = glfwCreateWindow(640, 480, "Tests", nullptr, nullptr);
        ASSERT_TRUE(window != nullptr);

        glfwMakeContextCurrent(window);

        // Load GL function pointers
        ASSERT_TRUE(gladLoadGL() != 0);
    }

    virtual void TearDown() {
        if (window) {
            glfwDestroyWindow(window);
        }
        glfwTerminate();
    }

private:
    GLFWwindow* window = nullptr;
};

static GLTestEnvironment* glEnv = nullptr;

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    glEnv = new GLTestEnvironment();
    ::testing::AddGlobalTestEnvironment(glEnv);
    return ::testing::UnitTest::GetInstance()->Run();
}
