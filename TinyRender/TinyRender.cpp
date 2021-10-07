#include <cmath>
#include <cstdlib>
#include <limits>

#include "TinyRender.h"

UINT32 white = 0xffffff;
UINT32 red = 0xff0000;
UINT32 green = 0x00ff00;
UINT32 blue = 0x0000ff;

#define COLOR(r, g, b) ((int(r) << 16) | ((int)(g) << 8) | (int)(b))

TinyRender::TinyRender(int width, int height)
	: _width(width),
	_height(height)
{
	model = new Model("obj/african_head.obj");
}

void TinyRender::render(UINT32** canvas)
{
	_canvas = canvas;

	/*line(0, 0, 0, 100, green);
	line(0, 0, 100, 0, green);
	line(0, 0, 100, 100, red);

	Vec2i t0[3] = { Vec2i(10, 70),   Vec2i(50, 160),  Vec2i(70, 80) };
	Vec2i t1[3] = { Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180) };
	Vec2i t2[3] = { Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180) };

	triangle(t0[0], t0[1], t0[2], red);
	triangle(t1[0], t1[1], t1[2], white);
	triangle(t2[0], t2[1], t2[2], green);*/

	//renderObj();

	renderObjWithTriangle();
}

void TinyRender::drawPoint(int x, int y, UINT32 color)
{
	_canvas[y][x] = color;
}

void TinyRender::line(int x0, int y0, int x1, int y1, UINT32 color)
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

void TinyRender::triangle(Vec2i t0, Vec2i t1, Vec2i t2, UINT32 color) {
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

void TinyRender::renderObj()
{
	int w = _width - 1;
	int h = _height - 1;
	for (int i = 0; i < model->nfaces(); i++)
	{
		std::vector<int> face = model->face(i);
		for (int j = 0; j < 3; j++)
		{
			Vec3f v0 = model->vert(face[j]);
			Vec3f v1 = model->vert(face[(j + 1) % 3]);
			int x0 = (v0.x + 1.0) * w / 2.0;
			int y0 = (v0.y + 1.0) * h / 2.0;
			int x1 = (v1.x + 1.0) * w / 2.0;
			int y1 = (v1.y + 1.0) * h / 2.0;
			line(x0, y0, x1, y1, white);
		}
	}
}


void TinyRender::triangleZBuff(Vec3f* pts, float* zbuffer, UINT32 color)
{
	Vec2f bboxmin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	Vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
	Vec2f clamp(_width - 1, _height - 1);
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 2; j++) {
			bboxmin[j] = std::max(0.f, std::min(bboxmin[j], pts[i][j]));
			bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
		}
	}
	Vec3f P;
	for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) {
		for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) {
			Vec3f bc_screen = barycentric(pts[0], pts[1], pts[2], P);
			if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) continue;
			P.z = 0;
			for (int i = 0; i < 3; i++) P.z += pts[i][2] * bc_screen[i];
			if (zbuffer[int(P.x + P.y * _width)] < P.z) {
				zbuffer[int(P.x + P.y * _width)] = P.z;
				drawPoint(P.x, P.y, color);
			}
		}
	}
}

Vec3f TinyRender::barycentric(Vec3f A, Vec3f B, Vec3f C, Vec3f P)
{
	Vec3f s[2];
	for (int i = 2; i--;)
	{
		s[i][0] = C[i] - A[i];
		s[i][1] = B[i] - A[i];
		s[i][2] = A[i] - P[i];
	}
	Vec3f u = cross(s[0], s[1]);
	if (std::abs(u[2]) > 1e-2)
	{
		return Vec3f(1.0f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
	}
	return Vec3f(-1, 1, 1);
}


void TinyRender::renderObjWithTriangle()
{
	int w = _width - 1;
	int h = _height - 1;
	/*Vec3f lightDir(0, 0, -1);
	for (int i = 0; i < model->nfaces(); i++)
	{
		std::vector<int> face = model->face(i);
		Vec2i screenCoords[3];
		Vec3f worldCorrds[3];
		for (int j = 0; j < 3; j++)
		{
			Vec3f v = model->vert(face[j]);
			screenCoords[j] = Vec2i((v.x + 1.0) * w / 2, (v.y + 1.0) * h / 2);
			worldCorrds[j] = v;
		}
		Vec3f n = cross((worldCorrds[2] - worldCorrds[0]), (worldCorrds[1] - worldCorrds[0]));
		n.normalize();
		float intensity = n * lightDir;
		if (intensity > 0)
		{
			triangle(screenCoords[0], screenCoords[1], screenCoords[2], COLOR(intensity * 255, intensity * 255, intensity * 255));
		}
	}*/
	Vec3f lightDir(0, 0, -1);
	float* zbuffer = new float[_width * _height];
	//float minF = -std::numeric_limits<float>::max();
	for (int i = _width * _height; i--; zbuffer[i] = -std::numeric_limits<float>::max());
	for (int i = 0; i < model->nfaces(); i++)
	{
		std::vector<int> face = model->face(i);
		Vec3f pts[3];
		for (int j = 0; j < 3; j++)
		{
			Vec3f v = model->vert(face[j]);
			pts[j] = Vec3f(int((v.x + 1.0) * w / 2 + 0.5), int((v.y + 1.0) * h / 2 + 0.5), v.z);
		}
		Vec3f n = cross((pts[1] - pts[0]), (pts[2] - pts[0]));
		n.normalize();
		float intensity = n * lightDir;
		if (intensity > 0)
		{
		}
			triangleZBuff(pts, zbuffer, COLOR(intensity * 255, intensity * 255, intensity * 255));
			//triangleZBuff(pts, zbuffer, COLOR(rand() % 255, rand() % 255, rand() % 255));
	}
}