#include <gtest/gtest.h>
#include <glad/glad.h>

#include "audio_manager_test_class.h"

TEST_F(AudioManagerTest, compilationTest) {
	int defaultVertexShaderSuccess;

	GLuint vertexShader;
	am.compileShader(fileToString("../shaders/default.vert").data(), vertexShader, GL_VERTEX_SHADER, defaultVertexShaderSuccess);

	EXPECT_TRUE(defaultVertexShaderSuccess);

	int defaultFragmentShaderSuccess;
	GLuint fragmentShader;
	am.compileShader(fileToString("../shaders/default.frag").data(), fragmentShader, GL_FRAGMENT_SHADER, defaultFragmentShaderSuccess);

	EXPECT_TRUE(defaultFragmentShaderSuccess);

	int doubleSymGeomShaderSuccess;
	GLuint symGeomShader;
	am.compileShader(fileToString("../shaders/symmetric.geom").data(), symGeomShader, GL_GEOMETRY_SHADER, doubleSymGeomShaderSuccess);

	EXPECT_TRUE(doubleSymGeomShaderSuccess);

	int defaultGeomShaderSuccess;
	GLuint defGeomShader;
	am.compileShader(fileToString("../shaders/default.geom").data(), defGeomShader, GL_GEOMETRY_SHADER, defaultGeomShaderSuccess);

	EXPECT_TRUE(defaultGeomShaderSuccess);

	int doublySymGeomShaderSuccess;
	GLuint dblSymGeomShader;
	am.compileShader(fileToString("../shaders/doubleSym.geom").data(), dblSymGeomShader, GL_GEOMETRY_SHADER, doublySymGeomShaderSuccess);

	EXPECT_TRUE(doublySymGeomShaderSuccess);
}