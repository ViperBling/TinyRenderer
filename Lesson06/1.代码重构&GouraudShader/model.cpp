/*
 * @Author: Zero
 * @Date: 2021-03-04 15:09:55
 * @LastEditTime: 2021-04-02 20:43:40
 * @Description: 
 */
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

#include "model.h"

Model::Model(const char *filename) : verts_(), faces_(), uvs_(), diffusemap_() {
    std::ifstream in(filename, std::ifstream::in);
    if (in.is_open()) {
        std::string line;
        while (std::getline(in, line)) {
            // 将字符串转换成字符输入流
            std::istringstream iss(line);
            char trash;                                     // 用于丢掉空格和/符号
            // 定位顶点数据位置，line下标为0-2的字符串和"v "比较
            if (line.substr(0, 2) == "v ") {
                iss >> trash;
                Vec3f v;
                for (int i = 0; i < 3; i++) iss >> v[i];    // 读取三个顶点
                verts_.emplace_back(v);
            } else if (line.substr(0, 3) == "vn "){         // 法向
                iss >> trash >> trash;
                Vec3f vn;
                for (int i = 0; i < 3; i++) iss >> vn[i];
                norms_.emplace_back(vn);
            } else if (line.substr(0, 3) == "vt ") {
                iss >> trash >> trash;
                Vec2f uv;
                for (int i = 0; i < 2; i++) iss >> uv[i];
                uvs_.emplace_back(uv);
            } else if (line.substr(0, 2) == "f ") {         // 定位三角面数据位置
                std::vector<Vec3i> f;
                Vec3i tmp;
                iss >> trash;
                while (iss >> tmp[0] >> trash >> tmp[1] >> trash >> tmp[2]) {
                    for (int i = 0; i < 3; i++) tmp[i]--;
                    f.emplace_back(tmp);
                }
                faces_.emplace_back(f);
            }
        }
    }
    loadTexture_(filename, "_diffuse.tga", diffusemap_);
    loadTexture_(filename, "_nm.tga", normalmap_);
    loadTexture_(filename, "_spec.tga", specularmap_);
    in.close();
    std::cerr << "# v# " << verts_.size() << " f# "  << faces_.size() << " vt# " << uvs_.size() << " vn# " << norms_.size() << std::endl;
}

Model::~Model() {}

int Model::nverts() {
    return (int)verts_.size();
}

int Model::nfaces() {
    return (int)faces_.size();
}

std::vector<int> Model::face(int idx) {
    std::vector<int> face;
    for (int i = 0; i < (int)faces_[idx].size(); i++) face.emplace_back(faces_[idx][i][0]);
    return face;
}

Vec3f Model::vert(int i) {
    return verts_[i];
}

Vec3f Model::vert(int iface, int nthvert) {
    return verts_[faces_[iface][nthvert][0]];
}

void Model::loadTexture_(std::string filename, const char* suffix, TGAImage& image) {
    std::string textfile(filename);
    size_t dot = textfile.find_last_of(".");
    if (dot != std::string::npos) {
        textfile = textfile.substr(0, dot) + std::string(suffix);
        std::cerr << "texture file " << textfile << " loading " << (image.read_tga_file(textfile.c_str()) ? "ok" : "failed") << std::endl;
        image.flip_vertically();
    }
}

TGAColor Model::diffuse(const Vec2f& uvf) {
    Vec2i uv(uvf[0] * diffusemap_.get_width(), uvf[1] * diffusemap_.get_height());
    return diffusemap_.get(uv[0], uv[1]);
}

Vec3f Model::normal(const Vec2f& uvf) {
    Vec2i uv(uvf[0] * normalmap_.get_width(), uvf[1] * normalmap_.get_height());
    TGAColor c = normalmap_.get(uv[0], uv[1]);
    Vec3f res;
    for (int i = 0; i < 3; i++) 
        res[2 - i] = (float)c[i] / 255.f * 2.f - 1.f;
    return res;
}

Vec3f Model::normal(int iface, int nthvert) {
    int idx = faces_[iface][nthvert][2];
    return norms_[idx].normalize();
}

Vec2f Model::uv(int iface, int nthvert) {
    return uvs_[faces_[iface][nthvert][1]];
}

float Model::specular(const Vec2f& uvf) {
    Vec2i uv(uvf[0] * specularmap_.get_width(), uvf[1] * specularmap_.get_height());
    return specularmap_.get(uv[0], uv[1])[0] / 1.f;
}
