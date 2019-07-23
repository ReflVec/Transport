// Transp.cpp : Описывает входную точку для приложения.
//

#include "stdafx.h"
#include "Transport.hpp"
#include "TransportTable.hpp"
#include "NumberEdit.hpp"
#include "functionsGUI.hpp"

#define MAX_LOADSTRING 100
#define TEXT_CHECK L"Проверка"

// Глобальные переменные:
HINSTANCE hInst;							// текущий экземпляр (current instance)
TCHAR szTitle[MAX_LOADSTRING];				// Текст заголовка (The title bar text)
TCHAR szWindowClass[MAX_LOADSTRING];		// Название класса главного окна (the main window class name)
WCHAR numberText[3];
const TCHAR *M_ERROR = L"Ошибка";					// Строка для заголовка окна оповещения
HWND hMainToolbar, hEditToolbar, hTable;	// две панели инструментов и транспортная таблица

bool new_file = true;
const int maxIter = 5000;
const uint32_t maxChar = 1024u;

// Объявления функций включенных в этот модуль:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	DBoxDistribute(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	AddBox(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	DeleteBox(HWND, UINT, WPARAM, LPARAM);

//
//  Функция: _tWinMain()
//
//	Назначение: Точка входа в программу
//
//	Комментарий:
//		С этой функции начинается выполнение кода этой программы
//
int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Разместить код сюда.
	MSG msg;
	HACCEL hAccelTable;

	// Инициализация глобальных строк
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_TRANSPORT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Выполнение инициализации пиложения:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TRANSPORT));

	// Главный цикл сообщений:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  Функция: MyRegisterClass()
//
//  Цель: Регистрация класса окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TRANSPORT));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_TRANSPORT);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   Функция: InitInstance(HINSTANCE, int)
//
//   Цель: Сохраняет экзепляр дескриптора и создаёт главное окно
//
//   Комментарий:
//
//        В этой функции, мы сохраняем экземпляр дескриптора в глобальной переменной и
//        создаём и отображаем главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   TransportTable::Initialize();

   hInst = hInstance; // Сохраняем экземпляр дескриптора в нашей глобальной пременной

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }
   
//   SetWindowLong(hWnd, GWL_EXSTYLE, WS_EX_LAYERED);
//   SetLayeredWindowAttributes(hWnd, 0, 220, LWA_ALPHA);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}


//
//  Функция: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Цель:  Обрабатывает сообщения для главного окна.
//
//  WM_COMMAND	- Обработка меню пиложения
//  WM_PAINT	- Отрисовка главного меню
//  WM_DESTROY	- Опубликовать сообщение о выходе и сделать возврат значения
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	//
	PAINTSTRUCT ps;
	HDC hdc = nullptr;

	MINMAXINFO *mmNewInfo;
	// 
	OPENFILENAME ofn;
	std::fstream fs;	
	// Результат действия
	LRESULT hResult;
	// Название файла, заголовок окна, расширение файла, фильтр для проводника:
	wchar_t fileName[maxChar], title[maxChar], buffer[maxChar], *extt = L".ttr", *ofnFilter = L"Transport table *.ttr\0;*.ttr\0All\0*.*\0";

	// Указатель на объект транспортной таблицы
	TransportTable *trTbl;
	// Данные для расчётов с транспортной таблицей:
	TableInfo tInfo;
	TransportData *Providers = nullptr, *Consumers = nullptr, *Costs = nullptr, *Transports = nullptr, transpRes;
	// По событиям:
	switch (message)
	{// При создании главного окна
	case WM_CREATE:
	{
		hTable = CreateWindowEx(WS_EX_CLIENTEDGE, L"TransportTable", L"TransportTableWindow",
			WS_VISIBLE | WS_CHILD | WS_VSCROLL | WS_HSCROLL, 0, 0, 640, 480,
			hWnd, nullptr, hInst, nullptr);
		
		// Панели инструментов:
		const DWORD buttonStyles = BTNS_AUTOSIZE;
		// Иконки панели инструментов
		DWORD icns[] = {
			IDI_NEWFILE,
			IDI_OPEN,
			IDI_SAVE,
			IDI_PREVIEW,
			IDI_SYNCHRONIZE,
			IDI_ZOOM,
			IDI_CURVEPOINTS,
			IDI_TODOLIST,
			IDI_CLOSEFILE,
			IDI_EXIT,
		};

		DWORD icnsd[] = {
			IDI_NEWFILEDISABLED,
			IDI_OPENDISABLED,
			IDI_SAVEDISABLED,
			IDI_PREVIEWDISABLED,
			IDI_SYNCHRONIZEDISABLED,
			IDI_ZOOM,
			IDI_CURVEPOINTSDISABLED,
			IDI_TODOLISTDISABLED,
			IDI_CLOSEFILEDISABLED,
			IDI_EXIT,
		};
		// Кнопки панели инструментов
		TBBUTTON tbButtons[] = {
			{ MAKELONG(0, 0), IDM_NEW,  TBSTATE_ENABLED, buttonStyles,{ 0 }, 0, (INT_PTR)L"Новый" },
			{ MAKELONG(1, 0), IDM_OPEN, TBSTATE_ENABLED, buttonStyles,{ 0 }, 0, (INT_PTR)L"Открыть" },
			{ MAKELONG(2, 0), IDM_SAVE, TBSTATE_INDETERMINATE, buttonStyles,{ 0 }, 0, (INT_PTR)L"Сохранить" },
			{ MAKELONG(3, 0), IDM_CALCULATE, TBSTATE_INDETERMINATE, buttonStyles,{ 0 }, 0, (INT_PTR)L"Рассчитать" },
			{ MAKELONG(4, 0), IDM_DISTRIBUTE, TBSTATE_INDETERMINATE, buttonStyles,{ 0 }, 0, (INT_PTR)L"Распределить" },
			{ MAKELONG(5, 0), IDM_CHECK, TBSTATE_INDETERMINATE, buttonStyles,{ 0 }, 0, (INT_PTR)L"Проверить" },
			{ MAKELONG(6, 0), IDM_OPTIMIZE, TBSTATE_INDETERMINATE, buttonStyles,{ 0 }, 0, (INT_PTR)L"Оптимизировать" },
			{ MAKELONG(7, 0), IDM_COST, TBSTATE_INDETERMINATE, buttonStyles,{ 0 }, 0, (INT_PTR)L"Рассчитать стоимость" },
			{ MAKELONG(8, 0), IDM_CLOSE, TBSTATE_INDETERMINATE, buttonStyles,{ 0 }, 0, (INT_PTR)L"Закрыть" },
			{ MAKELONG(9, 0), IDM_EXIT, TBSTATE_ENABLED, buttonStyles,{ 0 }, 0, (INT_PTR)L"Выход" },
		};

		hMainToolbar = TransportGUI::CreateToolbar(hInst, hWnd, 10, tbButtons, icns, icnsd, 24, WS_BORDER | TBSTYLE_TOOLTIPS);

		DWORD icnse[] = {
			IDI_ADD,
			IDI_DELETE,
		};

		DWORD icnsde[] = {
			IDI_ADDDISABLED,
			IDI_DELETEDISABLED,
		};

		TBBUTTON tbeButtons[] = {
			{ MAKELONG(0, 0), IDM_ADD,  TBSTATE_ENABLED, buttonStyles,{ 0 }, 0, 0 },
			{ MAKELONG(1, 0), IDM_DELETE, TBSTATE_ENABLED, buttonStyles,{ 0 }, 0, 0 },
		};

		hEditToolbar = TransportGUI::CreateToolbar(hInst, hWnd, 2, tbeButtons, icnse, icnsde, 16, WS_BORDER | TBSTYLE_WRAPABLE | CCS_NORESIZE);

		// Отключаем "Сохранить"
		EnableMenuItem(GetMenu(hWnd), IDM_SAVE, MF_DISABLED);
		// Отключаем кнопки для расчётов с транспортной таблицей
		TransportGUI::DeactivateCalculate(hWnd, hMainToolbar, hEditToolbar);
		// Отключаем "Закрыть"
		EnableMenuItem(GetMenu(hWnd), IDM_CLOSE, MF_DISABLED);
		break;
	}// Команды программы
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);

		trTbl = TransportTable::GetTransportCtrl(hTable);
		// Разбор выбора в меню:
		switch (wmId)
		{
		case IDM_NEW:
		{
			if (new_file) {
				trTbl->DeleteTable();
				trTbl->Update();

				GetWindowText(hWnd, title, 256);
				wsprintf(title, L"%s *Новый файл", title);
				SetWindowText(hWnd, title);

				SendMessage(hMainToolbar, TB_SETSTATE, (WPARAM)IDM_SAVE, (LPARAM)MAKELONG(TBSTATE_ENABLED, 0)); // Активировать кнопку
				EnableMenuItem(GetMenu(hWnd), IDM_SAVE, MF_ENABLED); // Строка "сохранить" становится активной

				SendMessage(hMainToolbar, TB_SETSTATE, (WPARAM)IDM_CLOSE, (LPARAM)MAKELONG(TBSTATE_ENABLED, 0)); // Активировать кнопку
				EnableMenuItem(GetMenu(hWnd), IDM_CLOSE, MF_ENABLED); // Строка "закрыть" становится активной
				
				SendMessage(hMainToolbar, TB_SETSTATE, (WPARAM)IDM_NEW, (LPARAM)MAKELONG(TBSTATE_INDETERMINATE, 0)); // Деактивировать кнопку
				EnableMenuItem(GetMenu(hWnd), IDM_NEW, MF_DISABLED); // Строка "новый файл" становится неактивной

				TransportGUI::ActivateCalculate(hWnd, hMainToolbar, hEditToolbar);

				new_file = false;
			}
			break;
		}
		case IDM_OPEN:
		{

			if (!new_file) {
				hResult = MessageBox(hWnd, L"Сохранить файл?", L"Выход из программы", MB_YESNOCANCEL | MB_ICONQUESTION);

				if (hResult == IDYES) {
					SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDM_SAVE, 0), 0);
				}
				else if (hResult == IDCANCEL) {
					break;
				}
			}

			ZeroMemory(&ofn, sizeof(OPENFILENAME));

			ofn.lpstrDefExt = extt;
			ofn.lpstrFilter = ofnFilter;

			ofn.nMaxFile = ARRAYSIZE(fileName);
			ofn.lpstrFile = fileName;

			TransportGUI::FillOFN(hWnd, ofn);

			if (GetOpenFileName(&ofn))
			{
				trTbl->DeleteTable();
				trTbl->Open(fileName);
				trTbl->Update();

				_wsplitpath(fileName, nullptr, nullptr, fileName, nullptr);
				LoadString(hInst, IDS_APP_TITLE, title, 256);
				wsprintf(title, L"%s %s", title, fileName);
				SetWindowText(hWnd, title);

				SendMessage(hMainToolbar, TB_SETSTATE, (WPARAM)IDM_SAVE, (LPARAM)MAKELONG(TBSTATE_ENABLED, 0)); // Активировать кнопку
				EnableMenuItem(GetMenu(hWnd), IDM_SAVE, MF_ENABLED); // Строка "сохранить" становится активной

				SendMessage(hMainToolbar, TB_SETSTATE, (WPARAM)IDM_NEW, (LPARAM)MAKELONG(TBSTATE_ENABLED, 0)); // Активировать кнопку
				EnableMenuItem(GetMenu(hWnd), IDM_NEW, MF_ENABLED); // Строка "новый файл" становится активной

				SendMessage(hMainToolbar, TB_SETSTATE, (WPARAM)IDM_CLOSE, (LPARAM)MAKELONG(TBSTATE_ENABLED, 0)); // Активировать кнопку
				EnableMenuItem(GetMenu(hWnd), IDM_CLOSE, MF_ENABLED); // Строка "Закрыть" становится активной

				TransportGUI::ActivateCalculate(hWnd, hMainToolbar, hEditToolbar);

				new_file = true;
			}
			break;
		}
		case IDM_SAVE: // При сохранении файла
		{
			ZeroMemory(&ofn, sizeof(OPENFILENAME));
			
			ofn.lpstrDefExt = extt;
			ofn.lpstrFilter = ofnFilter;

			ofn.nMaxFile = sizeof(fileName);
			ofn.lpstrFile = fileName;

			TransportGUI::FillOFN(hWnd, ofn);
            
			if (GetSaveFileName(&ofn))
			{
				if (trTbl->Save(fileName))
				{
					_wsplitpath(fileName, nullptr, nullptr, fileName, nullptr);
					LoadString(hInst, IDS_APP_TITLE, title, 256);
					wsprintf(title, L"%s %s", title, fileName);
					SetWindowText(hWnd, title);

//					EnableMenuItem(GetMenu(hWnd), IDM_SAVE, MF_DISABLED); // Строка "сохранить" не активна 
//					SendMessage(hMainToolbar, TB_SETSTATE, (WPARAM)IDM_SAVE, (LPARAM)MAKELONG(TBSTATE_INDETERMINATE, 0)); // Кнопка не активна

					SendMessage(hMainToolbar, TB_SETSTATE, (WPARAM)IDM_NEW, (LPARAM)MAKELONG(TBSTATE_ENABLED, 0)); // Активировать кнопку
					EnableMenuItem(GetMenu(hWnd), IDM_NEW, MF_ENABLED); // Строка "новый файл" становится активной

					new_file = true;
				}
			}
			break;
		}
		case IDM_CLOSE:
		{
			if (!new_file) {
				hResult = MessageBox(hWnd, L"Сохранить файл?", L"Выход из программы", MB_YESNOCANCEL | MB_ICONQUESTION);

				if (hResult == IDYES) {
					SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDM_SAVE, 0), 0);
				}
				else if (hResult == IDCANCEL) {
					break;
				}
			}

			LoadString(hInst, IDS_APP_TITLE, title, 256);
			SetWindowText(hWnd, title);

			EnableMenuItem(GetMenu(hWnd), IDM_SAVE, MF_DISABLED); // Строка "сохранить" не активна 
			SendMessage(hMainToolbar, TB_SETSTATE, (WPARAM)IDM_SAVE, (LPARAM)MAKELONG(TBSTATE_INDETERMINATE, 0)); // Кнопка не активна

			SendMessage(hMainToolbar, TB_SETSTATE, (WPARAM)IDM_NEW, (LPARAM)MAKELONG(TBSTATE_ENABLED, 0)); // Активировать кнопку
			EnableMenuItem(GetMenu(hWnd), IDM_NEW, MF_ENABLED); // Строка "новый файл" становится активной

			SendMessage(hMainToolbar, TB_SETSTATE, (WPARAM)IDM_CLOSE, (LPARAM)MAKELONG(TBSTATE_INDETERMINATE, 0));
			EnableMenuItem(GetMenu(hWnd), IDM_CLOSE, MF_DISABLED); // Строка "Закрыть" становится неактивной

			TransportGUI::DeactivateCalculate(hWnd, hMainToolbar, hEditToolbar);

			trTbl->DeleteTable();
			trTbl->Update();

			new_file = true;
			break;
		}
		case IDM_CALCULATE:
			hResult = SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDM_CHECK, 0), FALSE); // Проверяем транспортную таблицу

			if (trTbl->GetData(Providers, Consumers, Costs, Transports, tInfo) && hResult == TRUE)
			{
				for (int i = 0; i < maxIter; ++i)
				{
					if (!TC::Calculate(tInfo.Rows, tInfo.Columns, Transports, Costs)){
						break;
					}
				}

				trTbl->SetData(Transports);
				trTbl->Update();

				delete[] Providers, Consumers, Costs, Transports;
			}
			break;
		// Вызов диалогового окна 
		case IDM_DISTRIBUTE:
			new_file = false;
			DialogBox(hInst, MAKEINTRESOURCE(IDD_DISTRDIALOG), hWnd, DBoxDistribute);
			break;
		// Проверка отгрузок
		case IDM_CHECK:
			if (trTbl->GetData(Providers, Consumers, Costs, Transports, tInfo))
			{
				ZeroMemory(buffer, sizeof buffer);

				switch (TC::BalanceCheck(Providers, Consumers, tInfo.Rows, tInfo.Columns, transpRes))
				{
				case 1:
					if (MessageBox(hWnd, L"Таблица не сбалансирована. Добавить фиктивного потребителя?",
						TEXT_CHECK, MB_YESNO | MB_ICONQUESTION) == IDYES)
					{
						trTbl->AddColumns(1);
						swprintf(buffer, L"%g", transpRes);
						trTbl->SetColumnHeader(tInfo.Columns, buffer);
					}
					else {
						return FALSE;
					}
					break;
				case 2:
					if (MessageBox(hWnd, L"Таблица не сбалансирована. Добавить фиктивного поставщика?",
						TEXT_CHECK, MB_YESNO | MB_ICONQUESTION) == IDYES)
					{
						trTbl->AddRows(1);
						swprintf(buffer, L"%g", transpRes);
						trTbl->SetRowHeader(tInfo.Rows, buffer);
					}
					else {
						return FALSE;
					}
					break;
				default:
					if (TC::TransportCheck(Providers, Consumers, tInfo.Rows, tInfo.Columns, Transports)) {
						if(lParam != TRUE) MessageBox(hWnd, L"Отгрузки распределены правильно", TEXT_CHECK, MB_OK | MB_ICONASTERISK);
					}
					else {
						MessageBox(hWnd, L"Неправильно распределены отгрузки", TEXT_CHECK, MB_OK | MB_ICONASTERISK);
						return FALSE;
					}
					break;
				}

				trTbl->Update();

				delete[] Providers, Consumers, Costs, Transports;
			}
			return TRUE;
		// Оптимизация транспортной таблицы
		case IDM_OPTIMIZE:
			new_file = false;
			hResult = SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDM_CHECK, 0), TRUE); // Проверяем транспортную таблицу

			if (trTbl->GetData(Providers, Consumers, Costs, Transports, tInfo) && hResult == TRUE)
			{
				if(!TC::Calculate(tInfo.Rows, tInfo.Columns, Transports, Costs))
					MessageBox(hWnd, L"Это самый оптимальный план перевозок", TEXT_CHECK, MB_OK | MB_ICONASTERISK);

				trTbl->SetData(Transports);
				trTbl->Update();

				delete[] Providers, Consumers, Costs, Transports;
			}
			break;
		// Расчёт стоимости
		case IDM_COST:
			if (trTbl->GetData(Providers, Consumers, Costs, Transports, tInfo))
			{
				transpRes = TC::ShipCost(Transports, Costs, tInfo.Rows, tInfo.Columns);

				swprintf(buffer, L"Стоимость перевозок = %g", transpRes);
				MessageBox(hWnd, buffer, L"Стоимость транспортной таблицы", MB_OK | MB_ICONASTERISK);

				delete[] Providers, Consumers, Costs, Transports;
			}
			break;
		case IDM_ADD:
			new_file = false;
			DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_ADD), hWnd, AddBox);
			break;
		case IDM_DELETE:
			new_file = false;
			DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_DELETE), hWnd, DeleteBox);
			break;
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:

			if (!new_file) {
				hResult = MessageBox(hWnd, L"Сохранить файл?", L"Выход из программы", MB_YESNOCANCEL | MB_ICONQUESTION);

				if (hResult == IDYES) {
					SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDM_SAVE, 0), 0);
				}
				else if (hResult == IDCANCEL) {
					break;
				}
			}
			DestroyWindow(hWnd); // Закрытие окна программы
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
	{
		hdc = BeginPaint(hWnd, &ps);
		InvalidateRect(hTable, NULL, FALSE);
		EndPaint(hWnd, &ps);
		break;
	}
	//Получение информации о минимальном и максимальном размере окна:
	case WM_GETMINMAXINFO:
		mmNewInfo = reinterpret_cast<MINMAXINFO*>(lParam);//Преобразовываем указатель параметра в указатель на объект MINMAXINFO

		mmNewInfo->ptMinTrackSize.x = 800;//Минимальная ширина
		mmNewInfo->ptMinTrackSize.y = 600;//Минимальная высота
		break;
	case WM_SIZE:
	{
		int nWidth = LOWORD(lParam);  // ширина рабочей области 
		int nHeight = HIWORD(lParam); // высота рабочей области 
		
		SetWindowPos(hMainToolbar, NULL, 0, 0, nWidth-1, nHeight, SWP_NOOWNERZORDER);
		SetWindowPos(hEditToolbar, NULL, 0, 52, 26, nHeight - 52, SWP_SHOWWINDOW);
		SetWindowPos(hTable, NULL, 26, 52, nWidth-26, nHeight - 52, SWP_NOREDRAW);
		break;
	}
	case WM_ERASEBKGND:
		return 1;
	case WM_DESTROY:

		DestroyWindow(hTable);

		TransportTable::Release();
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


// Обработчик сообщений для about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

// Обработчик сообщений для Distribute box.
INT_PTR CALLBACK DBoxDistribute(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	WORD lwparam, hwparam;

	// Указатель на объект транспортной таблицы
	TransportTable *trTbl;
	// Данные для расчётов с транспортной таблицей:
	TableInfo tInfo;
	TransportData *Providers = nullptr, *Consumers = nullptr, *Costs = nullptr, *Transports = nullptr, transpRes;

	switch (message)
	{
	case WM_INITDIALOG:
	{
		SendDlgItemMessage(hDlg, IDC_DISTRCOMBO, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(L"Метод северо-западного угла"));
		SendDlgItemMessage(hDlg, IDC_DISTRCOMBO, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(L"Метод минимальных тарифов"));
		SendDlgItemMessage(hDlg, IDC_DISTRCOMBO, CB_SETCURSEL, 0, 0);

		return (INT_PTR)TRUE;
	}
	case WM_COMMAND:
		lwparam = LOWORD(wParam);
		hwparam = HIWORD(wParam);

		if (lwparam == IDOK || lwparam == IDCANCEL)// Обработка выбора пользователя
		{
			// Если подтвердили, то
			if (lwparam == IDOK)
			{
				trTbl = TransportTable::GetTransportCtrl(hTable);

				if (trTbl->GetData(Providers, Consumers, Costs, Transports, tInfo))///////////////////////////////
				{
					if (SendDlgItemMessage(hDlg, IDC_DISTRCOMBO, CB_GETCURSEL, 0, 0) == 0) {
						TC::NorthWest(Providers, Consumers, tInfo.Rows, tInfo.Columns, Transports);
					}
					else {
						TC::MinimalRate(Providers, Consumers, tInfo.Rows, tInfo.Columns, Costs, Transports);
					}

					trTbl->SetData(Transports);
					trTbl->Update();

					delete[] Providers, Consumers, Costs, Transports;/////////////////////////////////////////////////
				}
			}

			EndDialog(hDlg, lwparam);
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

// Обработчик сообщений для Add box.
INT_PTR CALLBACK AddBox(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	HWND hTmpEdit;
	WORD lwparam, hwparam;
	wchar_t buf[4];

	switch (message)
	{
	case WM_INITDIALOG:
	{
		HWND ProvEd, ConsEd, ProvSpin, ConsSpin;

		ProvEd = GetDlgItem(hDlg, IDC_EDIT_NUM_PROVIDERS);
		ConsEd = GetDlgItem(hDlg, IDC_EDIT_NUM_CONSUMERS);
		ProvSpin = GetDlgItem(hDlg, IDC_SPIN_NUM_PROVIDERS);
		ConsSpin = GetDlgItem(hDlg, IDC_SPIN_NUM_CONSUMERS);

		SetWindowText(ProvEd, L"0");
		SetWindowText(ConsEd, L"0");

		SendMessage(ProvEd, EM_LIMITTEXT, 3, 0);
		SendMessage(ConsEd, EM_LIMITTEXT, 3, 0);

		SendMessage(ProvSpin, UDM_SETRANGE32, 0, 100);
		SendMessage(ConsSpin, UDM_SETRANGE32, 0, 100);

		SendMessage(ProvSpin, UDM_SETBUDDY, reinterpret_cast<WPARAM>(ProvEd), 0);
		SendMessage(ConsSpin, UDM_SETBUDDY, reinterpret_cast<WPARAM>(ConsEd), 0);

		return (INT_PTR)TRUE;
	}
	case WM_COMMAND:
		lwparam = LOWORD(wParam);
		hwparam = HIWORD(wParam);

		// При взятии фокуса одним из полей редактирования
		if (hwparam == EN_SETFOCUS)//lwparam - IDC_EDIT...
		{
			switch (lwparam)
			{
			case IDC_EDIT_NUM_PROVIDERS:
			case IDC_EDIT_NUM_CONSUMERS:
				GetWindowText(reinterpret_cast<HWND>(lParam), numberText, 3); // Получаем строку
				break;
			default:
				break;
			}

		}
		else if (hwparam == EN_KILLFOCUS)// При убирании фокуса от поля ввода
		{
			hTmpEdit = reinterpret_cast<HWND>(lParam); // Получаем дескриптор окна
			NumericEdit<int> nmEd;

			switch (lwparam)
			{
			case IDC_EDIT_NUM_PROVIDERS:
				nmEd.SetHWNDs(hTmpEdit, GetDlgItem(hDlg, IDC_SPIN_NUM_PROVIDERS));
				break;
			case IDC_EDIT_NUM_CONSUMERS:
				nmEd.SetHWNDs(hTmpEdit, GetDlgItem(hDlg, IDC_SPIN_NUM_CONSUMERS));
				break;
			default:
				break;
			}

			// Проверяем введёные данные
			if (!nmEd.InputCheck(true))
				SetWindowText(hTmpEdit, numberText);

			// Сбрасываем nmEd
			nmEd.SetHWNDs(nullptr);
		}
		else if (lwparam == IDOK || lwparam == IDCANCEL)// Обработка выбора пользователя
		{
			// Если подтвердили, то
			if (lwparam == IDOK)
			{
				TransportTable *trTbl = TransportTable::GetTransportCtrl(hTable);
				UINT num = 0;

				SendDlgItemMessage(hDlg, IDC_EDIT_NUM_PROVIDERS, WM_GETTEXT, 4, reinterpret_cast<LPARAM>(buf));
				num = _wtoi(buf);

				trTbl->AddRows(num);

				SendDlgItemMessage(hDlg, IDC_EDIT_NUM_CONSUMERS, WM_GETTEXT, 4, reinterpret_cast<LPARAM>(buf));
				num = _wtoi(buf);

				trTbl->AddColumns(num);

				trTbl->Update();
			}

			EndDialog(hDlg, lwparam);
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

// Обработчик сообщений для Delete box.
INT_PTR CALLBACK DeleteBox(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	HWND hTmpEdit;
	WORD lwparam, hwparam;
	wchar_t buf[4];

	switch (message)
	{
	case WM_INITDIALOG:
	{
		HWND ProvEdFrom, ProvEdTo,
			 ConsEdFrom, ConsEdTo,
			 ProvSpinFrom, ProvSpinTo,
			 ConsSpinFrom, ConsSpinTo;
		
		ProvEdFrom = GetDlgItem(hDlg, IDC_EDIT_PROVIDERS_FROM);
		ProvEdTo = GetDlgItem(hDlg, IDC_EDIT_PROVIDERS_TO);
		ConsEdFrom = GetDlgItem(hDlg, IDC_EDIT_CONSUMERS_FROM);
		ConsEdTo = GetDlgItem(hDlg, IDC_EDIT_CONSUMERS_TO);

		ProvSpinFrom = GetDlgItem(hDlg, IDC_SPIN_PROVIDERS_FROM);
		ProvSpinTo = GetDlgItem(hDlg, IDC_SPIN_PROVIDERS_TO);
		ConsSpinFrom = GetDlgItem(hDlg, IDC_SPIN_CONSUMERS_FROM);
		ConsSpinTo = GetDlgItem(hDlg, IDC_SPIN_CONSUMERS_TO);

		SetWindowText(ProvEdFrom, L"1");
		SetWindowText(ConsEdFrom, L"1");
		SetWindowText(ProvEdTo, L"1");
		SetWindowText(ConsEdTo, L"1");

		SendMessage(ProvEdFrom, EM_LIMITTEXT, 3, 0);
		SendMessage(ProvEdTo, EM_LIMITTEXT, 3, 0);
		SendMessage(ConsEdFrom, EM_LIMITTEXT, 3, 0);
		SendMessage(ConsEdTo, EM_LIMITTEXT, 3, 0);

		SendMessage(ProvSpinFrom, UDM_SETRANGE32, 0, 100);
		SendMessage(ConsSpinFrom, UDM_SETRANGE32, 0, 100);
		SendMessage(ProvSpinTo, UDM_SETRANGE32, 0, 100);
		SendMessage(ConsSpinTo, UDM_SETRANGE32, 0, 100);

		SendMessage(ProvSpinFrom, UDM_SETBUDDY, reinterpret_cast<WPARAM>(ProvEdFrom), 0);
		SendMessage(ConsSpinFrom, UDM_SETBUDDY, reinterpret_cast<WPARAM>(ConsEdFrom), 0);
		SendMessage(ProvSpinTo, UDM_SETBUDDY, reinterpret_cast<WPARAM>(ProvEdTo), 0);
		SendMessage(ConsSpinTo, UDM_SETBUDDY, reinterpret_cast<WPARAM>(ConsEdTo), 0);

		return (INT_PTR)TRUE;
	}
	case WM_COMMAND:
		lwparam = LOWORD(wParam);
		hwparam = HIWORD(wParam);

		// При взятии фокуса одним из полей редактирования
		if (hwparam == EN_SETFOCUS)//lwparam - IDC_EDIT...
		{
			switch (lwparam)
			{
			case IDC_EDIT_PROVIDERS_FROM:
			case IDC_EDIT_PROVIDERS_TO:
			case IDC_EDIT_CONSUMERS_FROM:
			case IDC_EDIT_CONSUMERS_TO:
				GetWindowText(reinterpret_cast<HWND>(lParam), numberText, 3); // Получаем строку
				break;
			default:
				break;
			}

		}
		else if (hwparam == EN_KILLFOCUS)// При убирании фокуса от поля ввода
		{
			hTmpEdit = reinterpret_cast<HWND>(lParam); // Получаем дескриптор окна
			NumericEdit<int> nmEd;

			switch (lwparam)
			{
			case IDC_EDIT_PROVIDERS_FROM:
				nmEd.SetHWNDs(hTmpEdit, GetDlgItem(hDlg, IDC_SPIN_PROVIDERS_FROM));
				break;
			case IDC_EDIT_PROVIDERS_TO:
				nmEd.SetHWNDs(hTmpEdit, GetDlgItem(hDlg, IDC_SPIN_PROVIDERS_TO));
				break;
			case IDC_EDIT_CONSUMERS_FROM:
				nmEd.SetHWNDs(hTmpEdit, GetDlgItem(hDlg, IDC_SPIN_CONSUMERS_FROM));
				break;
			case IDC_EDIT_CONSUMERS_TO:
				nmEd.SetHWNDs(hTmpEdit, GetDlgItem(hDlg, IDC_SPIN_CONSUMERS_TO));
				break;
			default:
				break;
			}

			// Проверяем введёные данные
			if (!nmEd.InputCheck(true))
				SetWindowText(hTmpEdit, numberText);

			// Сбрасываем nmEd
			nmEd.SetHWNDs(nullptr);
		}else if (lwparam == IDOK || lwparam == IDCANCEL)
		{
			// Если подтвердили, то
			if (lwparam == IDOK)
			{
				TransportTable *trTbl = TransportTable::GetTransportCtrl(hTable);
				UINT from = 0, to = 0;
				// Поставщики
				SendDlgItemMessage(hDlg, IDC_EDIT_PROVIDERS_FROM, WM_GETTEXT, 4, reinterpret_cast<LPARAM>(buf));
				from = _wtoi(buf);

				SendDlgItemMessage(hDlg, IDC_EDIT_PROVIDERS_TO, WM_GETTEXT, 4, reinterpret_cast<LPARAM>(buf));
				to = _wtoi(buf);

				trTbl->DeleteRows(--from, --to);

				// Потребители
				SendDlgItemMessage(hDlg, IDC_EDIT_CONSUMERS_FROM, WM_GETTEXT, 4, reinterpret_cast<LPARAM>(buf));
				from = _wtoi(buf);

				SendDlgItemMessage(hDlg, IDC_EDIT_CONSUMERS_TO, WM_GETTEXT, 4, reinterpret_cast<LPARAM>(buf));
				to = _wtoi(buf);

				trTbl->DeleteColumns(--from, --to);

				trTbl->Update();
			}

			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}