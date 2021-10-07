#pragma once

#include <vector>
#include "Geometry.h"

class Model
{
private:
	std::vector<Vec3f> verts_;
	std::vector<std::vector<int>> faces_;

public:
	Model(const char* fileName);
	~Model();
	int nverts();
	int nfaces();
	Vec3f vert(int i);
	std::vector<int> face(int idx);
};