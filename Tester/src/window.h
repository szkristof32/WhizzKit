#pragma once

#include <stdint.h>
#include <Windows.h>

void CreateNewWindow(uint32_t width, uint32_t height, const wchar_t* title, HWND* window);
void PollWindowEvents(HWND window);
int WindowShouldClose(HWND window);