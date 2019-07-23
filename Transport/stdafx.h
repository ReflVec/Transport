// stdafx.h : файл включения стандартной системы включения файлов,
// или специфичные файлы включаемые в проект которые используются часто, но
// изменяются редко
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Исключение редко используемых вещей из Windows заголовка
// Заголовочные файлы Windows:
#include <windows.h>
#include <CommCtrl.h>
#include <CommDlg.h>

// Заголовочные файлы C RunTime
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

// Ссылки на остальные заоловки
#include <objidl.h>
#include <gdiplus.h>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")