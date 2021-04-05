/*
 * @Author: Zero
 * @Date: 2021-03-31 23:38:21
 * @LastEditTime: 2021-04-02 20:41:49
 * @Description: 
 */
#include <iostream>
#include <vector>
#include <cmath>
#include <limits>

#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
#include "our_gl.h"

const int width  = 800;
const int height = 800;

Model *model = nullptr;

Vec3f light_dir(1, 1, 1);
Vec3f       eye(1, 1, 3);
Vec3f    center(0, 0, 0);
Vec3f        up(0, 1, 0);

struct GouraudShader : public IShader {
    Vec3f varying_intensity;        // 由顶点着色器写入，被光栅化着色器读取

    virtual ~GouraudShader() {}

    virtual Vec4f vertex(int iface, int nthvert) {
        Vec4f gl_Vertex = embed<4>(model->vert(iface, nthvert));    // 从.obj文件中读取顶点
        gl_Vertex = ViewPort * Projection * ModelView * gl_Vertex;  // 坐标转换
        varying_intensity[nthvert] = std::max(0.f, model->normal(iface, nthvert) * light_dir);  // 获取漫反射强度
        return gl_Vertex;
    }

    virtual bool fragment(Vec3f bar, TGAColor& color) {
        float intensity = varying_intensity * bar;          // 对当前像素插值intensity
        color = TGAColor(255, 255, 255) * intensity;        // well duh
        return false;                                       // no, we do not discard this pixel
    }

    // virtual bool fragment(Vec3f bar, TGAColor& color) {
    //     float intensity = varying_intensity * bar;
    //     if (intensity > .85) intensity = 1;
    //     else if (intensity>.60) intensity = .80;
    //     else if (intensity>.45) intensity = .60;
    //     else if (intensity>.30) intensity = .45;
    //     else if (intensity>.15) intensity = .30;
    //     else intensity = 0;
    //     color = TGAColor(255, 155, 0)*intensity;
    //     return false;
    // }
};


int main(int argc, char** argv) { 
    if (2 == argc)
        model = new Model(argv[1]);
    else
        model = new Model("../obj/african_head.obj");
    
    lookat(eye, center, up);
    viewport(width / 8, height / 8, width * 3/4, height * 3/4);
    projection(-1.f / (eye - center).norm());
    light_dir.normalize();

    TGAImage image(width, height, TGAImage::RGB);
    TGAImage zbuffer(width, height, TGAImage::GRAYSCALE);

    GouraudShader shader;
    for (int i = 0; i < model->nfaces(); i++) {
        Vec4f screen_coords[3];
        for (int j = 0; j < 3; j++) {
            screen_coords[j] = shader.vertex(i, j);
        }
        triangle(screen_coords, shader, image, zbuffer);
    }

    image.flip_vertically();
    zbuffer.flip_vertically();
    image.write_tga_file("output.tga");
    zbuffer.write_tga_file("zbuffer.tga");
    
    delete model;
    return 0; 
}