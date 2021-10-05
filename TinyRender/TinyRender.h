#pragma once
#include <cstdlib>		// std::abs
#include <algorithm>	// std::swap

#include "WinApp.h"

UINT32 white = 0xffffff;
UINT32 red = 0xff0000;
UINT32 green = 0x00ff00;
UINT32 blue = 0x0000ff;

class TinyRender : public IRender
{
private:
	UINT32** _canvas;
public:
	virtual void render(UINT32** canvas)
	{
		_canvas = canvas;

		line(0, 0, 0, 100, green);
		line(0, 0, 100, 0, green);
		line(0, 0, 100, 100, red);
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
};