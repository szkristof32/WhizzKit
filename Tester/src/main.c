#include "window.h"

#include <WhizzKit/WhizzKit.h>
#include <WinUser.h>

int main()
{
	HWND window;
	CreateNewWindow(1600, 900, L"WhizzKit tester", &window);
	WzContext context;
	WzContextCreateInfo contextCreateInfo = { 0 };
	contextCreateInfo.contextType = WZ_CONTEXT_TYPE_OPENGL;
	contextCreateInfo.pNativeWindowHandle = window;
	wzCreateContext(&contextCreateInfo, &context);

	int running = 1;
	while (running)
	{
		PollWindowEvents(window);
		running = 1 - WindowShouldClose(window);

		wzSwapBuffers(context);
	}

	wzDestroyContext(context);
}