#pragma once

#include "stdafx.h"
#include "resource.h"

namespace TransportGUI
{
	//	Функция для создания простой панели инструментов
	HWND WINAPI CreateToolbar(HINSTANCE hInstance, HWND hWndParent, UINT numButtons, TBBUTTON *Buttons, DWORD *Icons, DWORD *disabledIcons, int bitmapSize = 32, DWORD Style = TBSTYLE_WRAPABLE);
	void AddIcons(HINSTANCE hInstance, UINT numIcons, HIMAGELIST hImgList, DWORD *Icons);

	BOOL FillOFN(HWND hWnd, OPENFILENAME &ofn);

	void ActivateCalculate(HWND hWnd, HWND TB_Main, HWND TB_Edit);
	void DeactivateCalculate(HWND hWnd, HWND TB_Main, HWND TB_Edit);

//	void ChangeSaveState();

	void Activate(HINSTANCE hInstance, HWND hWnd, DWORD Command);
	void Deactivate();
}