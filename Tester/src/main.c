#include <WhizzKit/WhizzKit.h>

int main()
{
	WzWindow window;
	WzWindowCreateInfo windowCreateInfo = { 0 };
	windowCreateInfo.width = 1600;
	windowCreateInfo.height = 900;
	windowCreateInfo.pTitle = "WhizzKit demo";
	windowCreateInfo.resizable = true;
	windowCreateInfo.windowPosition = WZ_WINDOW_POSITION_CENTER_SCREEN;
	wzCreateWindow(&windowCreateInfo, &window);

	WzOpenGLContext context;
	WzOpenGLContextCreateInfo contextCreateInfo = { 0 };
	contextCreateInfo.pWindowHandle = window;
	wzCreateOpenGLContext(&contextCreateInfo, &context);

	WzColour clearColour = {
		1.0f, 0.0f, 1.0f, 1.0f
	};

	while (wzWindowShouldClose(window))
	{
		wzPollEvents();

		wzClearScreenOpenGL(clearColour, WZ_CLEAR_COLOUR_BUFFER);
		wzSwapBuffersOpenGL(context);
	}

	wzDestroyOpenGLContext(context);
}