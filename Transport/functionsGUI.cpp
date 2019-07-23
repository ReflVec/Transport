#include "functionsGUI.hpp"

//-----------------------------------------------------------------------------------------------//
//	Функция для создания простой панели инструментов	-----------------------------------------//
//-----------------------------------------------------------------------------------------------//
HWND WINAPI TransportGUI::CreateToolbar(HINSTANCE hInstance, HWND hWndParent, UINT numButtons, TBBUTTON *Buttons, DWORD *Icons, DWORD *disabledIcons, int bitmapSize, DWORD Style)
{
	HWND hWndToolbar;
	HIMAGELIST hImgList, hDisabledImgList;

	// Объявление и инициализация локальных констант.
	//const int ImageListID = 0, DisabledImageListID = 1;

	Style |= WS_CHILD | WS_VISIBLE;

	// Создание панели инструментов.
	hWndToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL,
		Style, 0, 0, 0, 0,
		hWndParent, NULL, GetModuleHandle(NULL), NULL);

	if (hWndToolbar == NULL)
		return NULL;

	// Создание списка больших изображений.
	hImgList = ImageList_Create(bitmapSize, bitmapSize,   // Размеры отдельных растровых изображений.
		ILC_COLOR32 | ILC_MASK,   // Обеспечиваем прозрачный фон.
		numButtons, 0);

	// Создание списка маленьких изображений.
	hDisabledImgList = ImageList_Create(bitmapSize, bitmapSize,
		ILC_COLOR32 | ILC_MASK,
		numButtons, 0);

	AddIcons(hInstance, numButtons, hImgList, Icons);
	AddIcons(hInstance, numButtons, hDisabledImgList, disabledIcons);
		
	// Установить списки изображений для панели инструментов.
	SendMessage(hWndToolbar, TB_SETIMAGELIST,
		(WPARAM)0,
		(LPARAM)hImgList);

	SendMessage(hWndToolbar, TB_SETDISABLEDIMAGELIST,
		(WPARAM)1,
		(LPARAM)hDisabledImgList);

	// Загрузка изображений в кнопоки.
	SendMessage(hWndToolbar, TB_LOADIMAGES,
		(WPARAM)IDB_HIST_NORMAL,
		(LPARAM)hInstance);

	// Добавление кнопок.
	SendMessage(hWndToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
	SendMessage(hWndToolbar, TB_ADDBUTTONS, (WPARAM)numButtons, reinterpret_cast<LPARAM>(Buttons));

	// Изменение размера панели инструментов, и её отображение.
	SendMessage(hWndToolbar, TB_AUTOSIZE, 0, 0);
	ShowWindow(hWndToolbar, TRUE);

	return hWndToolbar;
}

void TransportGUI::AddIcons(HINSTANCE hInstance, UINT numIcons, HIMAGELIST hImgList, DWORD *Icons)
{
	HICON tempICO;

	if (Icons == nullptr || hImgList == nullptr)
		return;

	for (UINT i = 0; i < numIcons; ++i)
	{
		tempICO = LoadIcon(hInstance, MAKEINTRESOURCE(Icons[i]));
		ImageList_AddIcon(hImgList, tempICO);
		DestroyIcon(tempICO);
	}
}

void TransportGUI::ActivateCalculate(HWND hWnd, HWND TB_Main, HWND TB_Edit)
{
	SendMessage(TB_Main, TB_SETSTATE, (WPARAM)IDM_CALCULATE, (LPARAM)MAKELONG(TBSTATE_ENABLED, 0)); // Активировать кнопку
	EnableMenuItem(GetMenu(hWnd), IDM_CALCULATE, MF_ENABLED); // Строка "Расчёт" становится активной

	SendMessage(TB_Main, TB_SETSTATE, (WPARAM)IDM_DISTRIBUTE, (LPARAM)MAKELONG(TBSTATE_ENABLED, 0));
	EnableMenuItem(GetMenu(hWnd), IDM_DISTRIBUTE, MF_ENABLED);

	SendMessage(TB_Main, TB_SETSTATE, (WPARAM)IDM_CHECK, (LPARAM)MAKELONG(TBSTATE_ENABLED, 0));
	EnableMenuItem(GetMenu(hWnd), IDM_CHECK, MF_ENABLED);

	SendMessage(TB_Main, TB_SETSTATE, (WPARAM)IDM_OPTIMIZE, (LPARAM)MAKELONG(TBSTATE_ENABLED, 0));
	EnableMenuItem(GetMenu(hWnd), IDM_OPTIMIZE, MF_ENABLED);

	SendMessage(TB_Main, TB_SETSTATE, (WPARAM)IDM_COST, (LPARAM)MAKELONG(TBSTATE_ENABLED, 0));
	EnableMenuItem(GetMenu(hWnd), IDM_COST, MF_ENABLED);

	SendMessage(TB_Edit, TB_SETSTATE, (WPARAM)IDM_ADD, (LPARAM)MAKELONG(TBSTATE_ENABLED | TBSTATE_WRAP, 0));
	EnableMenuItem(GetMenu(hWnd), IDM_ADD, MF_ENABLED);

	SendMessage(TB_Edit, TB_SETSTATE, (WPARAM)IDM_DELETE, (LPARAM)MAKELONG(TBSTATE_ENABLED | TBSTATE_WRAP, 0));
	EnableMenuItem(GetMenu(hWnd), IDM_DELETE, MF_ENABLED);
};

void TransportGUI::DeactivateCalculate(HWND hWnd, HWND TB_Main, HWND TB_Edit)
{
	SendMessage(TB_Main, TB_SETSTATE, (WPARAM)IDM_CALCULATE, (LPARAM)MAKELONG(TBSTATE_INDETERMINATE, 0)); // Деактивировать кнопку
	EnableMenuItem(GetMenu(hWnd), IDM_CALCULATE, MF_DISABLED); // Строка "Расчёт" становится неактивной

	SendMessage(TB_Main, TB_SETSTATE, (WPARAM)IDM_DISTRIBUTE, (LPARAM)MAKELONG(TBSTATE_INDETERMINATE, 0));
	EnableMenuItem(GetMenu(hWnd), IDM_DISTRIBUTE, MF_DISABLED);

	SendMessage(TB_Main, TB_SETSTATE, (WPARAM)IDM_CHECK, (LPARAM)MAKELONG(TBSTATE_INDETERMINATE, 0));
	EnableMenuItem(GetMenu(hWnd), IDM_CHECK, MF_DISABLED);

	SendMessage(TB_Main, TB_SETSTATE, (WPARAM)IDM_OPTIMIZE, (LPARAM)MAKELONG(TBSTATE_INDETERMINATE, 0));
	EnableMenuItem(GetMenu(hWnd), IDM_OPTIMIZE, MF_DISABLED);

	SendMessage(TB_Main, TB_SETSTATE, (WPARAM)IDM_COST, (LPARAM)MAKELONG(TBSTATE_INDETERMINATE, 0));
	EnableMenuItem(GetMenu(hWnd), IDM_COST, MF_DISABLED);

	SendMessage(TB_Edit, TB_SETSTATE, (WPARAM)IDM_ADD, (LPARAM)MAKELONG(TBSTATE_INDETERMINATE | TBSTATE_WRAP, 0));
	EnableMenuItem(GetMenu(hWnd), IDM_ADD, MF_DISABLED);

	SendMessage(TB_Edit, TB_SETSTATE, (WPARAM)IDM_DELETE, (LPARAM)MAKELONG(TBSTATE_INDETERMINATE | TBSTATE_WRAP, 0));
	EnableMenuItem(GetMenu(hWnd), IDM_DELETE, MF_DISABLED);
};

BOOL TransportGUI::FillOFN(HWND hWnd, OPENFILENAME &ofn)
{
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.hInstance = GetModuleHandle(NULL);

	ofn.nFilterIndex = 0;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	ofn.lpstrFile[0] = '\0';

	return 0;
}

void TransportGUI::Activate(HINSTANCE hInstance, HWND hWnd, DWORD Command)
{

}

void TransportGUI::Deactivate()
{

}