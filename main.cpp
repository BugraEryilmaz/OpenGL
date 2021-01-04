#include <iostream>
#include <stdlib.h>
#include <cassert>
#include "linmath.h"
#include "parser.h"

//////-------- Global Variables -------/////////

GLuint gpuVertexBuffer;
GLuint gpuNormalBuffer;
GLuint gpuIndexBuffer;

// Sample usage for reading an XML scene file
parser::Scene scene;
static GLFWwindow* win = NULL;

static void errorCallback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void setVBOs() {
    

		glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);

		GLfloat* vertexPos = scene.vertex_data.data();  //vertices with xyz

		GLfloat* normals = scene.normal_data.data();  //triangle normal vectors

		GLuint* indices = scene.vertex_ids.data();  //triangle vertex ids
		

		GLuint vertexAttribBuffer, indexBuffer;

		glGenBuffers(1, &vertexAttribBuffer);
		glGenBuffers(1, &indexBuffer);

		assert(vertexAttribBuffer > 0 && indexBuffer > 0);

		glBindBuffer(GL_ARRAY_BUFFER, vertexAttribBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);


        int vertexPosSize = sizeof(GLfloat) * scene.vertex_data.size();
        int vertexNormalSize = sizeof(GLfloat) * scene.normal_data.size();
        int vertexIDSize = sizeof(GLuint) * scene.vertex_ids.size();
		
		glBufferData(GL_ARRAY_BUFFER, vertexPosSize + vertexNormalSize, 0, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertexPosSize, vertexPos);
		glBufferSubData(GL_ARRAY_BUFFER, vertexPosSize, vertexNormalSize, normals);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertexIDSize, indices, GL_STATIC_DRAW);
}

void drawMesh(parser::Mesh &mesh) {

	static bool firstTime = true;

	static int vertexPosDataSizeInBytes;
	
	if (firstTime)
	{
		firstTime = false;
	}

	glVertexPointer(3, GL_FLOAT, 0, 0);
	glColorPointer(3, GL_FLOAT, 0, reinterpret_cast<void*>(vertexPosDataSizeInBytes));
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

int main(int argc, char* argv[]) {
    scene.loadFromXml(argv[1]);

    glfwSetErrorCallback(errorCallback);

    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    win = glfwCreateWindow(scene.camera.image_width, scene.camera.image_height, "CENG477 - HW3", NULL, NULL);
    if (!win) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(win);

    GLenum err = glewInit();
    if (err != GLEW_OK) {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(win, keyCallback);

    //  color is determined by glmaterial code
    glEnable(GL_LIGHTING);
    glDisable(GL_COLOR_MATERIAL);

    while(!glfwWindowShouldClose(win)) {
        glfwWaitEvents();

        glfwSwapBuffers(win);
    }

    glfwDestroyWindow(win);
    glfwTerminate();

    exit(EXIT_SUCCESS);

    return 0;
}
