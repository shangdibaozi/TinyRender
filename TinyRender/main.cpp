
#include "WinApp.h"
#include "TinyRender.h"

int main()
{
	int width = 800;
	int height = 600;
	TinyRender tinyRender(width, height);
	WinApp app;
	app.create("TinyRender", width, height);
	app.run(tinyRender);
	return 0;
}

