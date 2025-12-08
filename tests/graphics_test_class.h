#define GLFW_INCLUDE_NONE

#include <gtest/gtest.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "audio_manager_test_class.h"

//	Graphics test class will create a glfw/OpenGL context when running setup
//	This way we have a fresh context for each independent test

class GraphicsTest : public AudioManagerTest {
public:
    bool hasWindow() const { return hasWindowCapability; }

protected:
	void SetUp() override {
        //  Setup the audiomanager
        AudioManagerTest::SetUp();

        ASSERT_TRUE(glfwInit());


        // Create hidden window for GL context
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        window = glfwCreateWindow(640, 480, "Tests", nullptr, nullptr);
        
        //  If no window is able to be created (ie Actions VM) we run different versions of Graphics Tests
        if (window == nullptr)
        {
            hasWindowCapability = false;
            return;
        }

        hasWindowCapability = true;
        glfwMakeContextCurrent(window);

        // Load GL function pointers
        ASSERT_TRUE(gladLoadGL() != 0);
	}

    void TearDown() override {
        if (window) {
            glfwDestroyWindow(window);
        }
        glfwTerminate();
    }
	
private:
	GLFWwindow* window = nullptr;
    bool hasWindowCapability;
};