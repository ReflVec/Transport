#pragma once

#include "stdafx.h"
#include "resource.h"
#include "NumberEdit.hpp"
#include "TransportCalculate.hpp"

//#define TTVM_SELECTEDIT1 = WM_USER + 0x01;

//template<typename TransportData> //м.б. и double


struct TableInfo {
	LONG Rows, Columns,
	Width, Height,
	SelRow, SelCol;
};

struct HeaderElement {
	LONG Col;
	RectF ioRC;
	TransportData hVal;
};

struct GridElement {
	LONG Row, Col;
	RectF TranspRC, costRC;
	TransportData tVal, cVal;
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
	static wchar_t buf[256];
	// GDI+
	static GdiplusStartupInput gdiplusStartupInput;
	static ULONG_PTR gdiplusToken;
	// Информация для отрисовки
	// Прямоугольники для определения областей
	RECT UpdRC;
	Rect dirtRC;

	// Элементы транспортной таблицы
	std::vector<GridElement> Elements;
	std::vector<HeaderElement> Consumers;
	std::vector<HeaderElement> Providers;

	// Шрифт
	Font *BigFont, *smallFont;
	// Цвета
	Color HeaderColor, HeaderTextColor, BorderColor, GridColor;
	// Информация о таблице
	TableInfo tblInfo;
	ScrollsInfo sInfo;
	Rect ColumnHeader, RowHeader;

	public:
		TransportTable(HWND hWnd);
//		TransportTable(UINT Rows, UINT Cols, UINT ColsWidth, UINT RowsHeight);

		static ATOM Initialize(); // 
		static BOOL Release(); // 

		HWND CreateTable(HINSTANCE hInst, HWND hWndParent, UINT x, UINT y, UINT Width, UINT Height, TableInfo &tInfo);

		bool AddRow();
		bool AddColumn();
		bool Set(UINT row, UINT col, wchar_t *pStr);

		bool AddRows(UINT num);
		bool AddColumns(UINT num);
		bool SetRows(UINT col, UINT from=0, UINT to=0);
		bool SetColumns(UINT row, UINT from=0, UINT to=0);

		bool DeleteRow(UINT row);
		bool DeleteColumn(UINT col);
		bool DeleteRows(UINT from, UINT to);
		bool DeleteColumns(UINT from, UINT to);

		bool SetColumnHeaders(wchar_t **cName, int Num);
		bool SetRowHeaders(wchar_t **rName, int Num);

//		bool GetItem(TransportData& Cost, TransportData& Transport);
		bool GetArrays(TransportData *A, TransportData *B, TransportData *Costs, TransportData *Transports);

		//bool ClearTable();
		bool DeleteTable();

		void Update();

		~TransportTable();

	private:
		// События
		LRESULT CALLBACK OnClick(WPARAM wParam, LPARAM lParam);
		LRESULT CALLBACK OnDoubleClick(WPARAM wParam, LPARAM lParam);
//		LRESULT CALLBACK OnSetFont(WPARAM wParam, LPARAM lParam);
		LRESULT CALLBACK OnPaint(WPARAM wParam, LPARAM lParam);
//		LRESULT CALLBACK OnMouseMove(WPARAM wParam, LPARAM lParam);
		LRESULT CALLBACK OnSize(WPARAM wParam, LPARAM lParam);
		BOOL CALLBACK OnVScroll(WPARAM wParam, LPARAM lParam);
		BOOL CALLBACK OnHScroll(WPARAM wParam, LPARAM lParam);
		// Обработчик событий окна транспортной таблицы
		static LRESULT CALLBACK TranspTblWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		// Обработчик событий для Edit
//		static LRESULT CALLBACK TTEditWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		void PlaceEdit(RectF &rc);

		void SetupScrollbars();
		bool PinToBottomCorner();

		void DrawGrid(Graphics *gp, Pen *pen, Rect &Borders);
		void DrawHeader(Graphics *gp, Rect &Borders);
		void DrawHeaderText(Graphics *gp);
		void HollowLine(Graphics *gp, Pen *pen1, Pen *pen2, Point *p1, Point *p2, int dir);//dir - direction 0 - all, 1 - horz., 2 - vert.
		void DrawSelect(Graphics *gp);
		void DrawTextElem(Graphics *gp);
};

void SetTransportCtrl(HWND hWnd, TransportTable *TranspTable);
TransportTable *GetTransportCtrl(HWND hWnd);