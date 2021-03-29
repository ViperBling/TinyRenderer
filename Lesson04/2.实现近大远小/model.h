#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "geometry.h"
#include "tgaimage.h"

class Model {
    
private:
	std::vector<Vec3f> verts_;
	std::vector<Vec3f> norms_;					// 法向
	std::vector<std::vector<Vec3i>> faces_;

	std::vector<Vec2f> uvs_;					// UV坐标
	TGAImage diffusemap_;						// 材质贴图
	void loadTexture_(std::string filename, const char * suffix, TGAImage& image);

public:
	Model(const char *filename);
	~Model();
	int nverts();								// 返回定点数
	int nfaces();								// 三角面的数量
	Vec3f vert(int i);							// 查询顶点
	Vec2i uv(int iface, int nvert);				// 查询uv，第iface个三角面的第nvert个顶点
	std::vector<int> face(int idx);				// 查询三角面
	TGAColor diffuse(const Vec2i& uv);			// 根据uv得到颜色
};

#endif //__MODEL_H__