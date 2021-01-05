#ifndef __HW1__PARSER__
#define __HW1__PARSER__

#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <math.h>

namespace parser
{
    //Notice that all the structures are as simple as possible
    //so that you are not enforced to adopt any style or design.
    /*
    struct Vec3f
    {
        float x, y, z;
    };
*/

struct Vec3f {
    double x, y, z;
    Vec3f operator+(const Vec3f& rhs)
    {
        Vec3f ret;
        ret.x = x + rhs.x;
        ret.y = y + rhs.y;
        ret.z = z + rhs.z;
        return ret;
    }
    Vec3f operator-(const Vec3f& rhs)
    {
        Vec3f ret;
        ret.x = x - rhs.x;
        ret.y = y - rhs.y;
        ret.z = z - rhs.z;
        return ret;
    }
    Vec3f operator*(double rhs)
    {
        Vec3f ret;
        ret.x = x * rhs;
        ret.y = y * rhs;
        ret.z = z * rhs;
        return ret;
    }
    Vec3f operator=(const Vec3f& rhs)
    {
        x = rhs.x;
        y = rhs.y;
        z = rhs.z;
        return *this;
    }
    Vec3f cross(Vec3f& rhs)
    {
        Vec3f ret;
        ret.x = y * rhs.z - z * rhs.y;
        ret.y = z * rhs.x - x * rhs.z;
        ret.z = x * rhs.y - y * rhs.x;
        return ret;
    }
    double dot(Vec3f& rhs)
    {
        return x * rhs.x + y * rhs.y + z * rhs.z;
    }
    Vec3f normalize()
    {
        double len;
        len = std::sqrt(x * x + y * y + z * z);
        Vec3f ret;
        ret.x = x / len;
        ret.y = y / len;
        ret.z = z / len;
        return ret;
    }
};

    struct Vec3i
    {
        int x, y, z;
    };

    struct Vec4f
    {
        float x, y, z, w;
    };

    struct Camera
    {
        Vec3f position;
        Vec3f gaze;
        Vec3f up;
        Vec4f near_plane;
        float near_distance;
        float far_distance;
        int image_width, image_height;
    };

    struct PointLight
    {
        Vec3f position;
        Vec3f intensity;
        bool status;
    };

    struct Material
    {
        Vec3f ambient;
        Vec3f diffuse;
        Vec3f specular;
        float phong_exponent;
    };

    struct Transformation
    {
        std::string transformation_type;
        int id;
    };

    struct Face
    {
        int v0_id;
        int v1_id;
        int v2_id;
    };

    struct Mesh
    {
        int material_id;
        int vertexIDstart;
        int numberofVertex;
        std::vector<Transformation> transformations;
        std::string mesh_type;
    };

    struct Scene
    {
        //Data
        Vec3i background_color;
        int culling_enabled;
        int culling_face;
        Camera camera;
        Vec3f ambient_light;
        std::vector<PointLight> point_lights;
        std::vector<Material> materials;
        std::vector<GLfloat> vertex_data;           
        //x, y, z coordinates 
        //[vertexID*3] = x 
        //[vertexID*3+1] = y
        //[vertexID*3+2] = z
        std::vector<GLuint> vertex_ids;             
        //vertex1ID vertex2ID vertex3ID triangles
        //[triangleID*3] = vertex1ID
        //[triangleID*3+1] = vertex2ID
        //[triangleID*3+2] = vertex3ID
        std::vector<GLfloat> normal_data;
        //x, y, z coordinates of normals 
        //[vertexID*3] = x
        //[vertexID*3+1] = y
        //[vertexID*3+2] = z
        std::vector<Vec3f> translations;
        std::vector<Vec3f> scalings;
        std::vector<Vec4f> rotations;
        std::vector<Mesh> meshes;

        //Functions
        void loadFromXml(const std::string& filepath);
    };
}

#endif