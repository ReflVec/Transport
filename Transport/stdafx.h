// stdafx.h : ���� ��������� ����������� ������� ��������� ������,
// ��� ����������� ����� ���������� � ������ ������� ������������ �����, ��
// ���������� �����
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // ���������� ����� ������������ ����� �� Windows ���������
// ������������ ����� Windows:
#include <windows.h>
#include <CommCtrl.h>
#include <CommDlg.h>

// ������������ ����� C RunTime
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

// ������ �� ��������� ��������
#include <objidl.h>
#include <gdiplus.h>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")