#pragma once
#include <cstdlib>		// std::abs
#include <algorithm>	// std::swap

#include "WinApp.h"
#include "Geometry.h"
#include "Model.h"

class TinyRender : public IRender
{
private:
	int _width;
	int _height;
	UINT32** _canvas = NULL;
	Model* model = NULL;
public:

	TinyRender(int width, int height);

	virtual void render(UINT32** canvas);

	void drawPoint(int x, int y, UINT32 color);

	void line(int x0, int y0, int x1, int y1, UINT32 color);

	void triangle(Vec2i t0, Vec2i t1, Vec2i t2, UINT32 color);

	void renderObj();

	void renderObjWithTriangle();
};