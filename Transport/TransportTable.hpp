#pragma once

#include "stdafx.h"
#include "resource.h"
#include "NumberEdit.hpp"
#include "TransportCalculate.hpp"

//#define TTVM_SELECTEDIT1 = WM_USER + 0x01;

//template<typename TransportData> //�.�. � double
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
	// ����
	HWND hWnd, hEdit;
	// ��� ������
	static LPCWSTR className;
	static wchar_t buf[48];
	// GDI+
	static GdiplusStartupInput gdiplusStartupInput;
	static ULONG_PTR gdiplusToken;
	// ���������� ��� ���������
	// �������������� ��� ����������� ��������
	RECT UpdRC;
	Rect dirtRC;

	// �������� ������������ �������
	std::vector<GridElement> Elements;
	std::vector<HeaderElement> Consumers;
	std::vector<HeaderElement> Providers;

	// �����
	Font *BigFont, *smallFont;
	HFONT hFnt; // ����� Edit
	// �����
	Color HeaderColor, HeaderTextColor, BorderColor, GridColor;
	// ���������� � �������
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

		// ������� ��������� �� ������ ������������� ����
		static void SetTransportCtrl(HWND hWnd, TransportTable *TranspTable);
		// ��������� ��������� �� ���������� ����
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
		// �������
		LRESULT CALLBACK OnClick(WPARAM wParam, LPARAM lParam); // ��� �������
		LRESULT CALLBACK OnDoubleClick(WPARAM wParam, LPARAM lParam); // ��� ������� �������
//		LRESULT CALLBACK OnSetFont(WPARAM wParam, LPARAM lParam); // ��� ������...
		LRESULT CALLBACK OnPaint(WPARAM wParam, LPARAM lParam); // ��� ���������
//		LRESULT CALLBACK OnMouseMove(WPARAM wParam, LPARAM lParam); // ��� �������� ����
		LRESULT CALLBACK OnSize(WPARAM wParam, LPARAM lParam); // ��� ��������� ������� ����
		BOOL CALLBACK OnVScroll(WPARAM wParam, LPARAM lParam); // ��� ������������ ����������������
		BOOL CALLBACK OnHScroll(WPARAM wParam, LPARAM lParam); // ��� ��������������

		// ���������� ������� ���� ������������ �������
		static LRESULT CALLBACK TranspTblWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		// ���������� ���� �������������� �� ������������ �������
		void PlaceEdit(RectF &rc, wchar_t *textVal);
		// ��������� ��������� �������
		void SetupScrollbars(); // ���������� ���������� ��������� �� ������ � ������ ��� ���������
		bool PinToBottomCorner(); // ���������� ������� ��� ����� ���������
		// ��������� ���������
		void DrawGrid(Graphics *gp, Pen *pen, Rect &Borders);
		void DrawHeader(Graphics *gp, Rect &Borders);
		void DrawHeaderText(Graphics *gp);
		void HollowLine(Graphics *gp, Pen *pen1, Pen *pen2, Point *p1, Point *p2, int dir);//dir - direction 0 - all, 1 - horz., 2 - vert.
		void DrawElements(Graphics *gp, Rect &Borders);
};