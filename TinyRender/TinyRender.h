#pragma once
#include <cstdlib>		// std::abs
#include <algorithm>	// std::swap

#include "WinApp.h"
#include "Geometry.h"

UINT32 white = 0xffffff;
UINT32 red = 0xff0000;
UINT32 green = 0x00ff00;
UINT32 blue = 0x0000ff;

class TinyRender : public IRender
{
private:
	int _width;
	int _height;
	UINT32** _canvas;
public:

	TinyRender(int width, int height)
		: _width(width),
		_height(height)
	{

	}

	virtual void render(UINT32** canvas)
	{
		_canvas = canvas;

		line(0, 0, 0, 100, green);
		line(0, 0, 100, 0, green);
		line(0, 0, 100, 100, red);

		Vec2i t0[3] = { Vec2i(10, 70),   Vec2i(50, 160),  Vec2i(70, 80) };
		Vec2i t1[3] = { Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180) };
		Vec2i t2[3] = { Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180) };

		triangle(t0[0], t0[1], t0[2], red);
		triangle(t1[0], t1[1], t1[2], white);
		triangle(t2[0], t2[1], t2[2], green);
	}

	void drawPoint(int x, int y, UINT32 color)
	{
		_canvas[y][x] = color;
	}

	void line(int x0, int y0, int x1, int y1, UINT32 color)
	{
		bool steep = false;
		if (std::abs(x0 - x1) < std::abs(y0 - y1))
		{
			std::swap(x0, y0);
			std::swap(x1, y1);
			steep = true;
		}
		if (x0 > x1)
		{
			std::swap(x0, x1);
			std::swap(y0, y1);
		}

		int dx = x1 - x0;
		int dy = y1 - y0;
		int derror2 = std::abs(dy) * 2;
		int error2 = 0;
		int y = y0;
		for (int x = x0; x <= x1; x++)
		{
			if (steep)
			{
				drawPoint(y, x, color);
			}
			else
			{
				drawPoint(x, y, color);
			}
			error2 += derror2;
			if (error2 > dx)
			{
				y += (y1 > y0 ? 1 : -1);
				error2 -= dx * 2;
			}
		}
	}

	void triangle(Vec2i t0, Vec2i t1, Vec2i t2, UINT32 color) {
		if (t0.y == t1.y && t0.y == t2.y)
		{
			return;
		}
		if (t0.y > t1.y)
		{
			std::swap(t0, t1);
		}
		if (t0.y > t2.y)
		{
			std::swap(t0, t2);
		}
		if (t1.y > t2.y)
		{
			std::swap(t1, t2);
		}
		int total_height = t2.y - t0.y;
		for (int i = 0; i < total_height; i++) {
			bool second_half = i > t1.y - t0.y || t1.y == t0.y;
			int segment_height = second_half ? t2.y - t1.y : t1.y - t0.y;
			float alpha = (float)i / total_height;
			float beta = (float)(i - (second_half ? t1.y - t0.y : 0)) / segment_height;
			Vec2i A = t0 + (t2 - t0) * alpha;
			Vec2i B = second_half ? t1 + (t2 - t1) * beta : t0 + (t1 - t0) * beta;
			if (A.x > B.x)
			{
				std::swap(A, B);
			}
			for (int j = A.x; j <= B.x; j++) {
				drawPoint(j, t0.y + i, color);
			}
		}
	}
};