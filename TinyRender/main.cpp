
#include "WinApp.h"
#include "TinyRender.h"

int main()
{
	TinyRender tinyRender;
	WinApp app;
	app.create("TinyRender", 800, 600);
	app.run(tinyRender);
	return 0;
}

