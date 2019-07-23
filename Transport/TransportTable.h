#pragma once

#include "stdafx.h"
#include "resource.h"

//#define TTVM_SELECTEDIT1 = WM_USER + 0x01;

//template<typename TransportData> //м.б. и double

struct TableInfo {
	LONG Rows, Columns,
	ColumnWidth, RowHeight,
	SelRow, SelCol;
};

struct ScrollsInfo {
	ULONG nVScrollPos, nHScrollPos,
		  nVScrollMax, nHScrollMax;
	int nWindowRows, nWindowColumns;
};

class TransportTable
{
	// Окна
	HWND hWnd, hEdit;
	// Имя класса
	static LPCWSTR className;
	// GDI+
	static GdiplusStartupInput gdiplusStartupInput;
	static ULONG_PTR gdiplusToken;
	// Информация для отрисовки
	// Прямоугольники для определения областей
	RECT UpdRC;
	Rect *TranspRects;
	// Шрифт
	Font *fontFam;
	// Цвета
	//Color cEdge, cSelEdge, cCell, cSelCell, cText, cSelText, 
	// Информация о таблице
	TableInfo tblInfo;
	ScrollsInfo sInfo;
	Rect ColumnHeader, RowHeader;
	//DECLARE_MESSAGE_MAP()
	public:
		TransportTable(HWND hWnd);
//		TransportTable(UINT Rows, UINT Cols, UINT ColsWidth, UINT RowsHeight);

		static ATOM Initialize(); // 
		static BOOL Release(); // 

		HWND CreateTable(HINSTANCE hInst, HWND hWndParent, UINT x, UINT y, UINT Width, UINT Height, TableInfo &tInfo);

		bool AddRow();
		bool AddColumn();
		bool SetRow();
		bool SetColumn();

		bool AddRows();
		bool AddColumns();
		bool SetRows();
		bool SetColumns();

		bool DeleteRow();
		bool DeleteColumn();
		bool DeleteRows();
		bool DeleteColumns();

		bool SetColumnHeaders(wchar_t **cName, int Num);
		bool SetRowHeaders(wchar_t **rName, int Num);

//		bool GetItem(TransportData& Cost, TransportData& Transport);
//		bool GetArrays(TransportData* A, TransportData* B, TransportData* Costs, TransportData* Transports);

		//bool ClearTable();
		bool DeleteTable();

		void Refresh();

		~TransportTable();

	private:
		// События
		LRESULT CALLBACK OnClick(WPARAM wParam, LPARAM lParam);
		LRESULT CALLBACK OnDoubleClick(WPARAM wParam, LPARAM lParam);
		LRESULT CALLBACK OnSetFont(WPARAM wParam, LPARAM lParam);
		LRESULT CALLBACK OnPaint(WPARAM wParam, LPARAM lParam);
		LRESULT CALLBACK OnSize(WPARAM wParam, LPARAM lParam);
		BOOL CALLBACK OnVScroll(WPARAM wParam, LPARAM lParam);
		BOOL CALLBACK OnHScroll(WPARAM wParam, LPARAM lParam);
		// Обработчик событий окна транспортной таблицы
		static LRESULT CALLBACK TranspTblWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		// Обработчики событий для Edit`ов
		static LRESULT CALLBACK TTEdit1WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		static LRESULT CALLBACK TTEdit2WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		void SetupScrollbars();
		bool PinToBottomCorner();
};

void SetTransportCtrl(HWND hWnd, TransportTable *TranspTable);
TransportTable *GetTransportCtrl(HWND hWnd);