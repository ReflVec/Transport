#pragma once

#include "stdafx.h"
#include "resource.h"
#include "NumberEdit.hpp"
#include "TransportCalculate.hpp"

//#define TTVM_SELECTEDIT1 = WM_USER + 0x01;

//template<typename TransportData> //м.б. и double
struct DrawInfo
{
	ULONG RowMin, RowMax,
		  ColMin, ColMax;
	LONG  RowCurSel, ColCurSel;
};

struct TableInfo {
	LONG Rows, Columns,
	Width, Height,
	SelRow, SelCol;
};

struct HeaderElement {
	wchar_t hVal[48];
};

struct GridElement {
	wchar_t tVal[48], cVal[48];
};

struct ScrollsInfo {
	ULONG nVScrollPos, nHScrollPos,
		  nVScrollMax, nHScrollMax;
	int	  nWindowRows, nWindowColumns;
};

class TransportTable
{
	// Окна
	HWND hWnd, hEdit;
	// Имя класса
	static LPCWSTR className;
	static wchar_t buf[48];
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
	HFONT hFnt; // шрифт Edit
	// Цвета
	Color HeaderColor, HeaderTextColor, BorderColor, GridColor;
	// Информация о таблице
	TableInfo tblInfo;
	ScrollsInfo sInfo;
	DrawInfo dInfo;
	Rect ColumnHeader, RowHeader, EditRC;
	std::vector<RectF> ConsRC, ProvRC, TranspRC, CostRC;

	public:
		TransportTable(HWND hWnd);
//		TransportTable(UINT Rows, UINT Cols, UINT ColsWidth, UINT RowsHeight);

		static ATOM Initialize(); // 
		static BOOL Release(); // 

		// Задание указателя на объект определенному окну
		static void SetTransportCtrl(HWND hWnd, TransportTable *TranspTable);
		// Получение указателя из информации окна
		static TransportTable *GetTransportCtrl(HWND hWnd);

		HWND CreateTable(HINSTANCE hInst, HWND hWndParent, UINT x, UINT y, UINT Width, UINT Height, TableInfo &tInfo);

		bool AddRow();
		bool AddColumn();

		bool AddRows(UINT num);
		bool AddColumns(UINT num);

		bool SetRowHeader(UINT row, wchar_t *hVal);
		bool SetColumnHeader(UINT col, wchar_t *hVal);

		bool DeleteRows(UINT from, UINT to);
		bool DeleteColumns(UINT from, UINT to);

		bool SetColumnHeaders(wchar_t **cName, int Num);
		bool SetRowHeaders(wchar_t **rName, int Num);

//		bool GetItem(TransportData& Cost, TransportData& Transport);
		TableInfo GetTableInfo();
		bool GetData(TransportData *&Prov, TransportData *&Cons, TransportData *&Costs, TransportData *&Transports, TableInfo &info);
		bool SetData(TransportData *Transports);

		bool Open(std::wstring fileName);
		bool Save(std::wstring fileName);

		//bool ClearTable();
		void DeleteTable();

		void Update();

		~TransportTable();

	private:
		// События
		LRESULT CALLBACK OnClick(WPARAM wParam, LPARAM lParam); // При нажатии
		LRESULT CALLBACK OnDoubleClick(WPARAM wParam, LPARAM lParam); // При двойном нажатии
//		LRESULT CALLBACK OnSetFont(WPARAM wParam, LPARAM lParam); // При вызове...
		LRESULT CALLBACK OnPaint(WPARAM wParam, LPARAM lParam); // При отрисовке
//		LRESULT CALLBACK OnMouseMove(WPARAM wParam, LPARAM lParam); // При движении мыши
		LRESULT CALLBACK OnSize(WPARAM wParam, LPARAM lParam); // При изменении размера окна
		BOOL CALLBACK OnVScroll(WPARAM wParam, LPARAM lParam); // При вертикальном позиционировании
		BOOL CALLBACK OnHScroll(WPARAM wParam, LPARAM lParam); // При горизонтальном

		// Обработчик событий окна транспортной таблицы
		static LRESULT CALLBACK TranspTblWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		// Размещение поля редактирования на транспортной таблице
		void PlaceEdit(RectF &rc, wchar_t *textVal);
		// Настройка прокрутки таблицы
		void SetupScrollbars(); // Установить количество элементов на экране и задать для прокрутки
		bool PinToBottomCorner(); // Перерасчёт позиции для полос прокрутки
		// Отрисовка элементов
		void DrawGrid(Graphics *gp, Pen *pen, Rect &Borders);
		void DrawHeader(Graphics *gp, Rect &Borders);
		void DrawHeaderText(Graphics *gp);
		void HollowLine(Graphics *gp, Pen *pen1, Pen *pen2, Point *p1, Point *p2, int dir);//dir - direction 0 - all, 1 - horz., 2 - vert.
		void DrawElements(Graphics *gp, Rect &Borders);
};