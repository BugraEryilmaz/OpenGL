#include <iostream>
#define _USE_MATH_DEFINES
#include <stdlib.h>
#include <cassert>
#include "linmath.h"
#define GLEW_STATIC 
#include "parser.h"
//////-------- Global Variables -------/////////

GLuint gpuVertexBuffer;
GLuint gpuNormalBuffer;
GLuint gpuIndexBuffer;
double fov;
double aspect;
double ndist;
double fdist;

// Sample usage for reading an XML scene file
parser::Scene scene;
static GLFWwindow* win = NULL;

static void errorCallback(int error, const char* description) {
    fprintf(stdout, "Error: %s\n", description);
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
    parser::Vec3f normal,sum; //merge
    for(i=0;i<scene.vertex_data.size()/3;i++){
        counter=0;
        sum.x=0;
        sum.y=0;
        sum.z=0;
        for(j=0;j<scene.vertex_ids.size()/3;j++){
            if(scene.vertex_ids[3*j]==i || scene.vertex_ids[3*j+1]==i || scene.vertex_ids[3*j+2]==i){
                counter++;
                vertex1.x =scene.vertex_data[3*scene.vertex_ids[3*j]]; //coordinate of first vertex of triangle
                vertex1.y =scene.vertex_data[3*scene.vertex_ids[3*j]+1];
                vertex1.z=scene.vertex_data[3*scene.vertex_ids[3*j]+2];
                vertex2.x=scene.vertex_data[3*scene.vertex_ids[3*j+1]]; //coordinate of second vertex of triangle
                vertex2.y=scene.vertex_data[3*scene.vertex_ids[3*j+1]+1];
                vertex2.z=scene.vertex_data[3*scene.vertex_ids[3*j+1]+2];
                vertex3.x=scene.vertex_data[3*scene.vertex_ids[3*j+2]]; //coordinate of third vertex of triangle
                vertex3.y=scene.vertex_data[3*scene.vertex_ids[3*j+2]+1];
                vertex3.z=scene.vertex_data[3*scene.vertex_ids[3*j+2]+2];
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

//glEnable(GL_CULL_FACE); 
//glCullFace(GL_FRONT);
//glCullFace(GL_BACK);

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
    int vertexPosSize = sizeof(GLfloat) * scene.vertex_data.size();
    int vertexNormalSize = sizeof(GLfloat) * scene.normal_data.size();
    int vertexIDSize = sizeof(GLuint) * scene.vertex_ids.size();

    parser::Camera camera = scene.camera;
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(camera.position.x, camera.position.y, camera.position.z, 
            camera.gaze.x + camera.position.x, camera.gaze.y + camera.position.y, camera.gaze.z + camera.position.z, 
            camera.up.x, camera.up.y, camera.up.z);
    for (int i = mesh.transformations.size()-1; i>=0;i--) {
        int id = mesh.transformations[i].id;
        if (mesh.transformations[i].transformation_type == "Translation") {
            glTranslatef(scene.translations[id-1].x,scene.translations[id-1].y,scene.translations[id-1].z);
        } else if (mesh.transformations[i].transformation_type == "Rotation") {
            glRotatef(scene.rotations[id-1].x,scene.rotations[id-1].y,scene.rotations[id-1].z,scene.rotations[id-1].w);
        } else if (mesh.transformations[i].transformation_type == "Scaling") {
            glScalef(scene.scalings[id-1].x,scene.scalings[id-1].y,scene.scalings[id-1].z);
        }
    }
    parser::Material material = scene.materials[mesh.material_id-1];
    GLfloat ambColor [ 4 ] = {material.ambient.x, material.ambient.y, material.ambient.z, 1.0 } ;
    GLfloat diffColor [ 4 ] = {material.diffuse.x, material.diffuse.y, material.diffuse.z, 1.0 } ;
    GLfloat specColor [ 4 ] = {material.specular.x, material.specular.y, material.specular.z, 1.0 } ;
    GLfloat specExp [ 1 ] = {material.phong_exponent};
    //GLfloat ambColor [ 4 ] = {0,0,0, 1.0 } ;
    //GLfloat diffColor [ 4 ] = {0,0,0, 1.0 } ;
    //GLfloat specColor [ 4 ] = {0,0,0, 1.0 } ;
    //GLfloat specExp [ 1 ] = {1};
    glMaterialfv ( GL_FRONT , GL_AMBIENT , ambColor ) ;
    glMaterialfv ( GL_FRONT , GL_DIFFUSE , diffColor ) ;
    glMaterialfv ( GL_FRONT , GL_SPECULAR , specColor ) ;
    glMaterialfv ( GL_FRONT , GL_SHININESS , specExp ) ;

	glVertexPointer(3, GL_FLOAT, 0, (void*)0);
    glNormalPointer(GL_FLOAT, 0, (void*)vertexPosSize);
	glDrawElements(GL_TRIANGLES, mesh.numberofVertex, GL_UNSIGNED_INT, (void*)(mesh.vertexIDstart*sizeof(GLuint)));
}

int main(int argc, char* argv[]) {
    scene.loadFromXml(argv[1]);
    normal_vec();
    glfwSetErrorCallback(errorCallback);

    if (!glfwInit()) {
        printf("GLFW init problem");
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    win = glfwCreateWindow(scene.camera.image_width, scene.camera.image_height, "CENG477 - HW3", NULL, NULL);
    if (!win) {
        printf("window problem");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(win);

    GLenum err = glewInit();
    if (err != GLEW_OK) {
        printf("GLEW problem");
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(win, keyCallback);

    glEnable(GL_SMOOTH);

    //  color is determined by glmaterial code
    glEnable(GL_LIGHTING);
    glDisable(GL_COLOR_MATERIAL);
    
    GLfloat light_ambient[] = { scene.ambient_light.x,scene.ambient_light.y,scene.ambient_light.z, 1.0 };
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glEnable(GL_LIGHT0);
    for (int i=0; i<scene.point_lights.size();i++) {
        GLfloat light_diffuse[] = { scene.point_lights[i].intensity.x, scene.point_lights[i].intensity.y, scene.point_lights[i].intensity.z, 1.0 };
        GLfloat light_position[] = { scene.point_lights[i].position.x, scene.point_lights[i].position.y, scene.point_lights[i].position.z, 1 };

        glLightfv(GL_LIGHT0+i+1, GL_DIFFUSE, light_diffuse);
        glLightfv(GL_LIGHT0+i+1, GL_SPECULAR, light_diffuse);
        //glLightf(GL_LIGHT0+i+1, GL_QUADRATIC_ATTENUATION, 1);
        glLightfv(GL_LIGHT0+i+1, GL_POSITION, light_position);
        glEnable(GL_LIGHT0+i+1);
    }
    fov = fabs(atan(scene.camera.near_plane.z/scene.camera.near_distance))+fabs(atan(scene.camera.near_plane.w/scene.camera.near_distance));
    fov = fov * 180 / M_PI;
    aspect = scene.camera.image_width/scene.camera.image_height;
    ndist = scene.camera.near_distance;
    fdist = scene.camera.far_distance;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov, aspect, ndist, fdist);
    setVBOs();
    while(!glfwWindowShouldClose(win)) {
        for (int i = 0; i < scene.meshes.size(); i++)
            drawMesh(scene.meshes[i]);
        glfwPollEvents();
        glfwSwapBuffers(win);
    }

    glfwDestroyWindow(win);
    glfwTerminate();
    exit(EXIT_SUCCESS);

    return 0;
}
