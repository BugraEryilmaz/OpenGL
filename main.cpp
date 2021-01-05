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

void normal_vec(){
    int i,j;
    int x,y,z;
    int counter;
    parser::Vec3f vertex1;
    parser::Vec3f  vertex2;
    parser::Vec3f  vertex3;
    parser::Vec3f triLine1, triLine2;
    parser::Vec3f normal,sum;
    sum.x=0;
    sum.y=0;
    sum.z=0;
    for(i=0;i<scene.vertex_data.size()/3;i++){
        counter=0;
        for(j=0;j<scene.vertex_ids.size()/3;j++){
            if(scene.vertex_ids[3*j]==i || scene.vertex_ids[3*j+1]==i || scene.vertex_ids[3*j+2]==i){
                counter++;
                vertex1.x =scene.vertex_data[scene.vertex_ids[3*j]]; //coordinate of first vertex of triangle
                vertex1.y =scene.vertex_data[scene.vertex_ids[3*j]+1];
                vertex1.z=scene.vertex_data[scene.vertex_ids[3*j]+2];
                vertex2.x=scene.vertex_data[scene.vertex_ids[3*j+1]]; //coordinate of second vertex of triangle
                vertex2.y=scene.vertex_data[scene.vertex_ids[3*j+1]+1];
                vertex2.z=scene.vertex_data[scene.vertex_ids[3*j+1]+2];
                vertex3.x=scene.vertex_data[scene.vertex_ids[3*j+2]]; //coordinate of third vertex of triangle
                vertex3.y=scene.vertex_data[scene.vertex_ids[3*j+2]+1];
                vertex3.z=scene.vertex_data[scene.vertex_ids[3*j+2]+2];
                triLine1=vertex2-vertex1; 
                triLine2=vertex3-vertex2;
                normal=triLine1.cross(triLine2).normalize();
                sum=sum+normal;
            }
        }
          scene.normal_data[3*i]=sum.x/counter;
          scene.normal_data[3*i+1]=sum.y/counter;
          scene.normal_data[3*i+2]=sum.z/counter;
    } 
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
