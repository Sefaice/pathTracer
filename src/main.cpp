// Copyright 2009-2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include <embree3/rtcore.h>
#include <OpenImageIO/imageio.h>
#include <stdio.h>
#include <math.h>
#include <limits>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>

#if defined(_WIN32)
#  include <conio.h>
#  include <windows.h>
#endif

#include "config.h"
#include "sampler.h"
#include "bsdf.h"
#include "light.h"

/* 
 * This is only required to make the tutorial compile even when
 * a custom namespace is set.
 */
#if defined(RTC_NAMESPACE_USE)
RTC_NAMESPACE_USE
#endif

struct Camera {
    vec3 pos;
    vec3 dst;
    vec3 up;
    float fov;
    float focusDist;

    Ray generateRay(Sampler *sampler, unsigned int y, unsigned int x, unsigned int height, unsigned int width) {
        // vec2 offset = sampler->sample2D(1)[0];
        // float xoff = x + offset.x;
        // float yoff = y + offset.y;
        float filmx = (float) x / width * 2.0 - 1.0;
        float filmy = (1.0 - (float)y / height) * 2.0 - 1.0;

        float ar = (float) width / height;
        float tanHalfHFOV = tanf(radians(fov / 2.0)) * ar;
	    float tanHalfVFOV = tanf(radians(fov / 2.0));

        vec3 raydir = vec3(filmx * tanHalfHFOV * focusDist, filmy * tanHalfVFOV * focusDist, -focusDist); // in view space
        raydir = normalize(raydir);
        vec4 raydir4 = vec4(raydir, 0);
        mat4 view = lookAt(pos, dst, up);

        raydir4 = inverse(view) * raydir4;
        raydir = vec3(raydir4.x, raydir4.y, raydir4.z); // in workd space
        
        return Ray(pos, raydir, 0, INF);
    }
};

/*
 * We will register this error handler with the device in initializeDevice(),
 * so that we are automatically informed on errors.
 * This is extremely helpful for finding bugs in your code, prevents you
 * from having to add explicit error checking to each Embree API call.
 */
void errorFunction(void* userPtr, enum RTCError error, const char* str)
{
  printf("error %d: %s\n", error, str);
}

unsigned int initTriangle(RTCDevice device, RTCScene scene) {
    RTCGeometry geom = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);
    float* vertices = (float*) rtcSetNewGeometryBuffer(geom, RTC_BUFFER_TYPE_VERTEX, 0, 
        RTC_FORMAT_FLOAT3, 3*sizeof(float), 3);
    unsigned* indices = (unsigned*) rtcSetNewGeometryBuffer(geom, RTC_BUFFER_TYPE_INDEX, 0,
        RTC_FORMAT_UINT3, 3*sizeof(unsigned), 1);

    vertices[0] = -1.0f; vertices[1] = -1.0f; vertices[2] = 0.f;
    vertices[3] = 1.f; vertices[4] = 0.f; vertices[5] = 0.5f;
    vertices[6] = 0.f; vertices[7] = 1.f; vertices[8] = -2.0f;
    indices[0] = 0; indices[1] = 1; indices[2] = 2;

    rtcCommitGeometry(geom);
    unsigned int geomID = rtcAttachGeometry(scene, geom);
    rtcReleaseGeometry(geom);
    return geomID;
}

unsigned int initGroundPlane(RTCDevice device, RTCScene scene) {
    /* create a triangulated plane with 2 triangles and 4 vertices */
    RTCGeometry mesh = rtcNewGeometry (device, RTC_GEOMETRY_TYPE_TRIANGLE);
    float* vertices = (float*) rtcSetNewGeometryBuffer(mesh,RTC_BUFFER_TYPE_VERTEX,0,
        RTC_FORMAT_FLOAT3, 3*sizeof(float), 4);
    unsigned* indices = (unsigned*) rtcSetNewGeometryBuffer(mesh, RTC_BUFFER_TYPE_INDEX, 0,
        RTC_FORMAT_UINT3, 3*sizeof(unsigned), 2);

    vertices[0] = -10; vertices[1] = -2; vertices[2] = -10;
    vertices[3] = -10; vertices[4] = -2; vertices[5] = +10;
    vertices[6] = +10; vertices[7] = -2; vertices[8] = -10;
    vertices[9] = +10; vertices[10] = -2; vertices[11] = +10;
    indices[0] = 0; indices[1] = 1; indices[2] = 2;
    indices[3] = 1; indices[4] = 3; indices[5] = 2;

    rtcCommitGeometry(mesh);
    unsigned int geomID = rtcAttachGeometry(scene,mesh);
    rtcReleaseGeometry(mesh);
    return geomID;
}

unsigned int initCube(RTCDevice device, RTCScene scene) {
    /* create a triangulated cube with 12 triangles and 8 vertices */
    RTCGeometry mesh = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);
    vec3 face_colors[12];
    vec3 vertex_colors[8];
    float* vertices = (float*) rtcSetNewGeometryBuffer(mesh,RTC_BUFFER_TYPE_VERTEX,0,
        RTC_FORMAT_FLOAT3, 3 * sizeof(float), 8);
    unsigned* indices = (unsigned*) rtcSetNewGeometryBuffer(mesh, RTC_BUFFER_TYPE_INDEX, 0,
        RTC_FORMAT_UINT3, 3*sizeof(unsigned), 12);
    rtcSetGeometryVertexAttributeCount(mesh,1);
    rtcSetSharedGeometryBuffer(mesh,RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE,0,
        RTC_FORMAT_FLOAT3,vertex_colors,0,sizeof(vec3),8);

    vertex_colors[0] = vec3(0,0,0); vertices[0] = -1; vertices[1] = -1; vertices[2] = -1;
    vertex_colors[1] = vec3(0,0,1); vertices[3] = -1; vertices[4] = -1; vertices[5] = +1;
    vertex_colors[2] = vec3(0,1,0); vertices[6] = -1; vertices[7] = +1; vertices[8] = -1;
    vertex_colors[3] = vec3(0,1,1); vertices[9] = -1; vertices[10] = +1; vertices[11] = +1;
    vertex_colors[4] = vec3(1,0,0); vertices[12] = +1; vertices[13] = -1; vertices[14] = -1;
    vertex_colors[5] = vec3(1,0,1); vertices[15] = +1; vertices[16] = -1; vertices[17] = +1;
    vertex_colors[6] = vec3(1,1,0); vertices[18] = +1; vertices[19] = +1; vertices[20] = -1;
    vertex_colors[7] = vec3(1,1,1); vertices[21] = +1; vertices[22] = +1; vertices[23] = +1;
    // left side
    face_colors[0] = vec3(1,0,0); indices[0] = 0; indices[1] = 1; indices[2] = 2;
    face_colors[1] = vec3(1,0,0); indices[3] = 1; indices[4] = 3; indices[5] = 2;
    // right side
    face_colors[2] = vec3(0,1,0); indices[6] = 4; indices[7] = 6; indices[8] = 5;
    face_colors[3] = vec3(0,1,0); indices[9] = 5; indices[10] = 6; indices[11] = 7;
    // bottom side
    face_colors[4] = vec3(0.5f); indices[12] = 0; indices[13] = 4; indices[14] = 1;
    face_colors[5] = vec3(0.5f); indices[15] = 1; indices[16] = 4; indices[17] = 5;
    // top side
    face_colors[6] = vec3(1.0f); indices[18] = 2; indices[19] = 3; indices[20] = 6;
    face_colors[7] = vec3(1.0f); indices[21] = 3; indices[22] = 7; indices[23] = 6;
    // front side
    face_colors[8] = vec3(0,0,1); indices[24] = 0; indices[25] = 2; indices[26] = 4;
    face_colors[9] = vec3(0,0,1); indices[27] = 2; indices[28] = 6; indices[29] = 4;
    // back side
    face_colors[10] = vec3(1,1,0); indices[30] = 1; indices[31] = 5; indices[32] = 3;
    face_colors[11] = vec3(1,1,0); indices[33] = 3; indices[34] = 5; indices[35] = 7;

    rtcCommitGeometry(mesh);
    unsigned int geomID = rtcAttachGeometry(scene,mesh);
    rtcReleaseGeometry(mesh);
    return geomID;
}

void waitForKeyPressedUnderWindows()
{
#if defined(_WIN32)
  HANDLE hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
  
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  if (!GetConsoleScreenBufferInfo(hStdOutput, &csbi)) {
    printf("GetConsoleScreenBufferInfo failed: %d\n", GetLastError());
    return;
  }
  
  /* do not pause when running on a shell */
  if (csbi.dwCursorPosition.X != 0 || csbi.dwCursorPosition.Y != 0)
    return;
  
  /* only pause if running in separate console window. */
  printf("\n\tPress any key to exit...\n");
  int ch = getch();
#endif
}

class Application {
public:
    /* RTC */
    RTCDevice device;
    RTCScene scene;
    /* framebuffer settings */
    const int width = 640;
    const int height = 480;
    const int channels = 3;
    unsigned char *pixels; // top-left origin
    /* */
    unsigned int cubeID;
    unsigned int groundID;

    Application() {
        // init device
        device = rtcNewDevice(NULL);
        if (!device)
            printf("error %d: cannot create device\n", rtcGetDeviceError(NULL));
        rtcSetDeviceErrorFunction(device, errorFunction, NULL);

        // init scene
        scene = rtcNewScene(device);
        //initTriangle(device, scene);
        cubeID = initCube(device, scene);
        groundID = initGroundPlane(device, scene);
        rtcCommitScene(scene);

        pixels = new unsigned char[width * height * channels];
    }

    ~Application() {
        rtcReleaseScene(scene);
        rtcReleaseDevice(device);
    }

    void renderToFile(Camera camera, Sampler *sampler, Light *light, unsigned int spp, const std::string& fileName) {
        render(camera, sampler, light, spp);
        storeImage(fileName);
    }

    void render(Camera camera, Sampler *sampler, Light *light, unsigned int spp) {
        for (unsigned int y = 0; y < height; y++) {
            for (unsigned int x = 0; x < width; x++) {

                vec3 accColor = vec3(0);
                for (unsigned int s = 0; s < spp; s++) {
                    accColor = accColor + renderPixel(camera, sampler, light, y, x);
                }

                accColor = accColor / (float)spp;
                pixels[y * width * channels + x * channels] = 255 * accColor.x;
                pixels[y * width * channels + x * channels + 1] = 255 * accColor.y;
                pixels[y * width * channels + x * channels + 2] = 255 * accColor.z;
            }
        }
    }

    vec3 renderPixel(Camera camera, Sampler *sampler, Light *light, unsigned int y, unsigned int x) {

        vec3 L = vec3(0.0);
        int depth = 0;
        Ray ray = camera.generateRay(sampler, y, x, height, width);

        while (depth < 10) {

            struct RTCIntersectContext context;
            rtcInitIntersectContext(&context);
            struct RTCRayHit rayhit;
            rayhit.ray.org_x = ray.pos.x; rayhit.ray.org_y = ray.pos.y; rayhit.ray.org_z = ray.pos.z;
            rayhit.ray.dir_x = ray.dir.x; rayhit.ray.dir_y = ray.dir.y; rayhit.ray.dir_z = ray.dir.z;
            rayhit.ray.tnear = ray.min_t; rayhit.ray.tfar = ray.max_t;
            rayhit.ray.mask = -1; rayhit.ray.flags = 0;
            rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
            rayhit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;
            rtcIntersect1(scene, &context, &rayhit);

            if (rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID) {

                ray.pos = ray.pos + ray.dir * rayhit.ray.tfar;
        
                //L = vec3(rayhit.hit.u, rayhit.hit.v, 1 - rayhit.hit.u - rayhit.hit.v);
                vec3 normal = vec3(rayhit.hit.Ng_x, rayhit.hit.Ng_y, rayhit.hit.Ng_z);
                normal = normalize(normal);

                // sample BSDF
                vec3 wi;
                float pdf;
                vec3 tmp1 = Sample_BSDF(normal, ray.dir, &wi, sampler->sample2D(1)[0], &pdf);
                // if (rayhit.hit.geomID == cubeID) {
                //     L = normal;
                // } else if (rayhit.hit.geomID == groundID) {
                //     L = vec3(0.5, 0.1, 0.1);
                // }

                // sample light
                vec3 wiL;
                float pdfL;
                vec3 tmp2 = light->Sample_Li(ray.pos, sampler->sample2D(1)[0], &wiL, &pdfL);
                //// shadow ray
                //Ray shadowRay(ray.pos, wi, 0.001f, INF);
                //rtcOccluded1(scene, &context, (RTCRay*)&shadowRay);
                //if (shadowRay.max_t > 0.0f) { // no intersection with scene
                //    //L = L + vec3(1) * tmp1 / pdf;
                //    //L = L + tmp2 / pdfL;

                //    // // intersect with light
                //    // struct RTCRayHit rayhitlight;
                //    // rayhitlight.ray.org_x = ray.pos.x; rayhitlight.ray.org_y = ray.pos.y; rayhitlight.ray.org_z = ray.pos.z;
                //    // rayhitlight.ray.dir_x = wi.x; rayhitlight.ray.dir_y = wi.y; rayhitlight.ray.dir_z = wi.z;
                //    // rayhitlight.ray.tnear = ray.min_t; rayhitlight.ray.tfar = ray.max_t;
                //    // rayhitlight.ray.mask = -1; rayhitlight.ray.flags = 0;
                //    // rayhitlight.hit.geomID = RTC_INVALID_GEOMETRY_ID;
                //    // rayhitlight.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;
                //    // rtcIntersect1(scene, &context, &rayhitlight);
                //    pdfL = light->Pdf_Li(ray.pos, , , wi);
                //    L = L + vec3(1) / pdfL;
                //}

                // update ray
                ray.dir = normalize(wi);
                ray.min_t = 0;
                ray.max_t = INF;

            } else { // no intersection with scene
                break;
            }

            depth++;
        }

        return L;
    }

    void storeImage(const std::string& fileName) {
        using namespace OIIO;

        std::unique_ptr<ImageOutput> out = ImageOutput::create(fileName.c_str());
        if (!out)
            return;
        ImageSpec spec(width, height, channels, TypeDesc::UINT8);
        out->open(fileName.c_str(), spec);
        out->write_image(TypeDesc::UINT8, pixels);
        out->close ();
    }
};

/* -------------------------------------------------------------------------- */

int main() {
    std::cout << "Version: " << pathTracer_VERSION_MAJOR << "." << pathTracer_VERSION_MINOR << std::endl;

    unsigned int spp = 4;

    Camera camera;
    camera.pos = vec3(5, 20, 20);
    camera.dst = vec3(0, 0, 0);
    camera.up = vec3(0, 1, 0);
    camera.fov = 45.0;
    camera.focusDist = 1.0;

    //Light *light = new PointLight(vec3(-5, 5, -5), vec3(1));
    // disk area light is at (-5, 5, -5) face down
    mat4 diskObjectToWorld(1.0);
    rotate(diskObjectToWorld, radians(180.0), vec3(1, 0, 0));
    translate(diskObjectToWorld, vec3(-5, 5, -5));
    mat4 diskWorldToObject = inverse(diskObjectToWorld);
    Disk *disk = new Disk(2, &diskObjectToWorld, &diskWorldToObject);
    Light *light = new DiffuseAreaLight(disk, vec3(1));

    Sampler *sampler = new Sampler();

    Application app;
    app.renderToFile(camera, sampler, light, spp, "hello11.png");

    // /* wait for user input under Windows when opened in separate window */
    // waitForKeyPressedUnderWindows();

    return 0;
}

