#include "TransportTable.hpp"

//using namespace Gdiplus;

LPCWSTR TransportTable::className = L"TransportTable";
wchar_t TransportTable::buf[256] = { 0 };

// GDI+
GdiplusStartupInput TransportTable::gdiplusStartupInput;
ULONG_PTR TransportTable::gdiplusToken;

TransportTable *GetTransportCtrl(HWND hWnd)
{
	return reinterpret_cast<TransportTable*>(GetWindowLong(hWnd, 0));// Заменить на static_cast<TransportTable*>(reinterpret_cast<void*>())
}

void SetTransportCtrl(HWND hWnd, TransportTable *TranspTable)
{
	//SetWindowLong(hWnd, 0, (LONG)TranspTable);
	SetWindowLong(hWnd, 0, reinterpret_cast<LONG>(TranspTable));// Заменить на static_cast<LONG>(reinterpret_cast<void*>())
}

void TransportTable::HollowLine(Graphics *gp, Pen *pen1, Pen *pen2, Point *p1, Point *p2, int dir) {

	if (gp != nullptr || pen1 != nullptr || pen2 != nullptr || p1 != nullptr || p2 != nullptr)
	{
		Point pt1 = *p1, pt2 = *p2;

		gp->DrawLine(pen1, pt1, pt2);

		if (dir == 1) {
			pt1.X--; pt2.X--;
		}
		else if (dir == 2) {
			pt1.Y--; pt2.Y--;
		}

		gp->DrawLine(pen2, pt1, pt2);

		if (dir == 1) {
			pt1.X += 2; pt2.X += 2;
		}
		else if (dir == 2) {
			pt1.Y += 2; pt2.Y += 2;
		}

		gp->DrawLine(pen2, pt1, pt2);
	}
}

void TransportTable::DrawSelect(Graphics * gp)
{
	SolidBrush BF(Color(80, 0, 0, 255));

	wchar_t buf[256] = { L"A:B" }, ch1[1] = { 'A' }, ch2[1] = { 'B' };
	int len = 3;

	SolidBrush penText(Color(255, 255, 255));

	StringFormat sf(StringFormatFlagsLineLimit);
	sf.SetAlignment(StringAlignmentCenter);
	sf.SetTrimming(StringTrimmingCharacter);
	sf.SetLineAlignment(StringAlignmentCenter);

	for (int c = 0; c < tblInfo.Columns; ++c) {
		if (tblInfo.SelCol == c)
			gp->FillRectangle(&BF, Consumers[c].ioRC);

		gp->DrawString(ch1, 1, BigFont, Consumers[c].ioRC, &sf, &penText);
	}

	for (int r = 0; r < tblInfo.Rows; ++r) {
		if (tblInfo.SelRow == r)
			gp->FillRectangle(&BF, Providers[r].ioRC);

		gp->DrawString(ch2, 1, BigFont, Providers[r].ioRC, &sf, &penText);
	}

	size_t idx;

	for (size_t y = 0; y < tblInfo.Rows; ++y)
		for (size_t x = 0; x < tblInfo.Columns; ++x) {
			idx = y * tblInfo.Columns + x;

			if(tblInfo.SelCol == x && tblInfo.SelRow == y)
				gp->FillRectangle(&BF, Elements[idx].costRC);
		}
}

void TransportTable::DrawTextElem(Graphics * gp)
{

}

void TransportTable::DrawGrid(Graphics *gp, Pen *pen, Rect &Borders) {
	Point pt1X, pt2X, pt1Y, pt2Y;
	Rect rate;

	if (gp != nullptr || pen != nullptr)
	{
		rate.Width = tblInfo.Width / 2;
		rate.Height = tblInfo.Height / 2;

		for (int x = 0; x < tblInfo.Columns + 1; ++x)
		{
			pt1X.X = pt2X.X = Borders.X + x*tblInfo.Width;
			pt1X.Y = ColumnHeader.Height;
			pt2X.Y = Borders.Height;

			gp->DrawLine(pen, pt1X, pt2X);
		}

		for (int y = 0; y < tblInfo.Rows + 1; ++y)
		{
			pt1Y.Y = pt2Y.Y = Borders.Y + y*tblInfo.Height;
			pt1Y.X = RowHeader.Width;
			pt2Y.X = Borders.Width;

			gp->DrawLine(pen, pt1Y, pt2Y);
		}

		for (int x = 0; x < tblInfo.Columns; ++x)
			for (int y = 0; y < tblInfo.Rows; ++y)
			{
				rate.X = Borders.X + rate.Width*x + rate.Width*(x+1);
				rate.Y = Borders.Y + rate.Height*y + rate.Height*y;
				gp->DrawRectangle(pen, rate);
			}
		//POINTS MAKEPOINTS()
	}
}

void TransportTable::DrawHeader(Graphics *gp, Rect &Borders)
{
	Point pt1, pt2;
	Rect HeaderRC;

	pt1.Y = 0;
	pt2.Y = ColumnHeader.Height;

	SolidBrush HeaderBrush(this->HeaderColor);
	Pen HeaderPen(this->HeaderColor);
	Pen pTop(Color(90, 240, 230));
	Pen pBottom(Color(3, 133, 123));

	if (gp) {
		HeaderRC.Width = tblInfo.Width + RowHeader.Width;
		HeaderRC.Height = RowHeader.Height;

		gp->FillRectangle(&HeaderBrush, HeaderRC);
		gp->DrawRectangle(&pBottom, HeaderRC);

		HeaderRC = Borders;
		HeaderRC.Height = ColumnHeader.Height;

		gp->FillRectangle(&HeaderBrush, HeaderRC);

		for (int c = 0; c < tblInfo.Columns + 1; ++c)
		{
			pt1.X = pt2.X = RowHeader.Width + ColumnHeader.Width + c*ColumnHeader.Width;

			if (c > 0)
				HollowLine(gp, &HeaderPen, &pBottom, &pt1, &pt2, 1);
			else
				gp->DrawLine(&pBottom, pt1, pt2);
		}

		HeaderRC.Height = Borders.Height;
		HeaderRC.Width = RowHeader.Width;

		pt1.X = 0;
		pt2.X = RowHeader.Width;

		gp->FillRectangle(&HeaderBrush, HeaderRC);

		for (int r = 0; r < tblInfo.Rows + 1; ++r)
		{
			pt1.Y = pt2.Y = RowHeader.Height + r*RowHeader.Height;

			if (r > 0)
				HollowLine(gp, &HeaderPen, &pBottom, &pt1, &pt2, 2);
			else
				gp->DrawLine(&pBottom, pt1, pt2);
		}

		pt1 = pt2 = Point(tblInfo.Width + RowHeader.Width, ColumnHeader.Height);
		pt2.X = Borders.Width;

		gp->DrawLine(&pBottom, pt1, pt2);

		pt1 = pt2 = Point(RowHeader.Width, RowHeader.Height);
		pt2.Y = Borders.Height;

		gp->DrawLine(&pBottom, pt1, pt2);
	}
}

void TransportTable::DrawHeaderText(Graphics *gp)
{
	wchar_t buf[256] = { L"A:B" }, ch1[1] = { 'A' }, ch2[1] = { 'B' };
	int len = 3;

	RectF trc, src;
	PointF pStr;
	SolidBrush penText(HeaderTextColor);

	StringFormat sf(StringFormatFlagsLineLimit);
	sf.SetAlignment(StringAlignmentCenter);
	sf.SetTrimming(StringTrimmingCharacter);
	sf.SetLineAlignment(StringAlignmentCenter);

	trc.Width = RowHeader.Width + tblInfo.Width;
	trc.Height = tblInfo.Height;

	if (gp) {
		gp->DrawString(buf, len, BigFont, trc, &sf, &penText);

		trc.Width = tblInfo.Width;
		trc.Height = ColumnHeader.Height;

		for (int c = 0; c < tblInfo.Columns; ++c)
		{
			trc.X = RowHeader.Width + ColumnHeader.Width + c*ColumnHeader.Width;

			swprintf(buf, L"%d", c + 1);
			len = wcslen(buf);

			gp->DrawString(ch2, 1, BigFont, trc, &sf, &penText);
			//backBuffer.DrawString(buf, len, fontFam, trc, &sf, &bckgnd);
			gp->MeasureString(buf, len, BigFont, pStr, &src);

			trc.X += src.Width / 2 + 1;
			gp->DrawString(buf, len, smallFont, trc, &sf, &penText);
		}

		trc.X = 0;
		trc.Width = RowHeader.Width;
		trc.Height = tblInfo.Height;

		for (int r = 0; r < tblInfo.Rows; ++r)
		{
			trc.Y = RowHeader.Height + r*RowHeader.Height;

			swprintf(buf, L"%d", r + 1);
			len = wcslen(buf);

			gp->DrawString(ch1, 1, BigFont, trc, &sf, &penText);
			//backBuffer.DrawString(buf, len, fontFam, trc, &sf, &bckgnd);
			gp->MeasureString(buf, len, BigFont, pStr, &src);

			src.X = trc.Width / 2 + 1;
			src.Y = trc.Y + (trc.Height - src.Height) / 2;
			gp->DrawString(buf, len, smallFont, src, &sf, &penText);
		}
	}
}

LRESULT TransportTable::OnClick(WPARAM wParam, LPARAM lParam)
{
	POINTS pts = MAKEPOINTS(lParam);
	tblInfo.SelCol = tblInfo.SelRow = -1;
	
	for (int y = 0; y < tblInfo.Rows; ++y) {

		if (Providers[y].ioRC.Contains(pts.x, pts.y))
			tblInfo.SelRow = y;

		for (int x = 0; x < tblInfo.Columns; ++x)
		{
			if (Consumers[x].ioRC.Contains(pts.x, pts.y))
				tblInfo.SelCol = x;

			if (Elements[y*tblInfo.Columns + x].costRC.Contains(pts.x, pts.y)) {
				tblInfo.SelCol = x; tblInfo.SelRow = y;
			}
		}
	}

	//InvalidateRect(this->hWnd, NULL, FALSE);
	//SendMessage(this->hWnd, WM_PAINT, 0, 0);
	this->OnPaint(0, 1);
	SetFocus(this->hWnd);
	//Update();

	return 0;
}

LRESULT TransportTable::OnDoubleClick(WPARAM wParam, LPARAM lParam)
{
	if (!(tblInfo.SelRow < 0) && tblInfo.SelCol < 0) {
		PlaceEdit(Providers[tblInfo.SelRow].ioRC);
	};	
	
	if (!(tblInfo.SelCol < 0) && tblInfo.SelRow < 0) {
		PlaceEdit(Consumers[tblInfo.SelCol].ioRC);
	};

	if (!(tblInfo.SelCol < 0) && !(tblInfo.SelRow < 0)) {
		PlaceEdit(Elements[tblInfo.SelRow*tblInfo.Columns +tblInfo.SelCol].costRC);
	};

	return 0;
}

LRESULT TransportTable::OnPaint(WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	RECT rc;

	Rect hit, GridBorders; Point mp; SolidBrush sel(Color(150, 0, 160, 255));

	if (lParam == 1)
		hdc = GetDC(this->hWnd);
	else
		hdc = BeginPaint(this->hWnd, &ps);

	//ps.rcPaint;
	GetClientRect(this->hWnd, &rc);
	Rect drc, HdrRC, HdrGradRC;
	memcpy(&drc, &rc, sizeof(RECT));

	Graphics ttcd(hdc);

	ttcd.GetClipBounds(&dirtRC);//

	// Double buffered rendering
	Bitmap BackImg(drc.Width, drc.Height, &ttcd);
	Graphics backBuffer(&BackImg);

	SolidBrush bckgnd(Color(255, 255, 255));
	Pen GridPen(this->GridColor);

	backBuffer.FillRectangle(&bckgnd, drc);

	hit.X = drc.X;
	hit.Y = drc.Y;
	hit.Width = drc.Width;
	hit.Height = drc.Height;
/*
	if (hit.Contains(LOWORD(lParam), HIWORD(lParam)))
	{
		backBuffer.Clear(Color(255, 255, 255));
		backBuffer.FillRectangle(&sel, hit);
	}
*/
	GridBorders.X = RowHeader.Width + tblInfo.Width;
	GridBorders.Y = ColumnHeader.Height * 2;
	GridBorders.Width = drc.Width;
	GridBorders.Height = drc.Height;

	DrawGrid(&backBuffer, &GridPen, GridBorders);
	DrawHeader(&backBuffer, drc);
	DrawHeaderText(&backBuffer);

	/*Check*/
	DrawSelect(&backBuffer);
	/**/

	// Present results
	ttcd.DrawImage(&BackImg, drc);
	
	if (lParam == 1)
		ReleaseDC(this->hWnd, hdc);
	else
		EndPaint(this->hWnd, &ps);

	return 0;
}

LRESULT TransportTable::OnSize(WPARAM wParam, LPARAM lParam)
{
	RECT rc;

	GetWindowRect(hWnd, &rc);

//	SetWindowPos(hEdit, NULL, 0, 0, 0, 0, SWP_NOSIZE);

	UpdRC = rc;
	UpdRC.top += tblInfo.Height/2;
	UpdRC.left += tblInfo.Width/2;

	sInfo.nWindowRows = min((rc.bottom - rc.top) / tblInfo.Height, tblInfo.Rows);
	sInfo.nWindowColumns = min((rc.right - rc.left) / tblInfo.Width, tblInfo.Columns);

	if (PinToBottomCorner())
		UpdateWindow(hWnd);

	SetupScrollbars();

	return LRESULT();
}

BOOL TransportTable::OnVScroll(WPARAM wParam, LPARAM lParam)
{
	BOOL bResult = TRUE;/////////////////////////////////////////////////////////////////////////////////////
	SCROLLINFO scrInfo;
	LONG oldY, newY;

	scrInfo.cbSize = sizeof(scrInfo);
	scrInfo.fMask = SIF_ALL;
	
	::GetScrollInfo(hWnd, SB_VERT, &scrInfo);
	oldY = scrInfo.nPos;

	switch (LOWORD(wParam))
	{
	// Пользователь нажал клавишу Home
	case SB_TOP:
		scrInfo.nPos = scrInfo.nMin;
		break;
	// Пользователь нажал клавишу End
	case SB_BOTTOM:
		scrInfo.nPos = scrInfo.nMax;
		break;
	// Пользователь нажал верхнюю стрелку
	case SB_LINEUP:
		--scrInfo.nPos;
		break;
	// Пользователь нажал нижнюю стрелку
	case SB_LINEDOWN:
		++scrInfo.nPos;
		break;
	// Пользователь нажал на место над движком
	case SB_PAGEUP:
		scrInfo.nPos -= scrInfo.nPage;
		break;
	// Пользователь нажал на место под движком
	case SB_PAGEDOWN:
		scrInfo.nPos += scrInfo.nPage;
		break;
	// Пользователь перетащил scroll box
	case SB_THUMBTRACK:
		scrInfo.nPos = scrInfo.nTrackPos/*tblInfo.RowHeight*/; // Позиция scroll box`а
		break;
	default:
		break;
	}
	// Установим позицию и потом возвратим её.
	// В связи с корректировками Windows оно не может быть таким же, как установленное значение.
	scrInfo.fMask = SIF_POS;
	SetScrollInfo(this->hWnd, SB_VERT, &scrInfo, TRUE);
	GetScrollInfo(this->hWnd, SB_VERT, &scrInfo);
	
	// Если позиция изменилась, листаем окно и обновляем его
	if (scrInfo.nPos != oldY)
	{
		bResult = ScrollWindowEx(this->hWnd, 0, tblInfo.Height*(oldY - scrInfo.nPos), 
								 NULL, NULL, NULL, NULL, SW_ERASE | SW_INVALIDATE);////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		UpdateWindow(this->hWnd);
	}

	return bResult;
}

LRESULT CALLBACK TransportTable::TranspTblWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;

	TransportTable *CustTranspTable;
	CustTranspTable = GetTransportCtrl(hWnd);

	switch (message)
	{
	case WM_NCCREATE:
		CustTranspTable = new TransportTable(hWnd);
		SetTransportCtrl(hWnd, CustTranspTable);

		if (CustTranspTable != nullptr)
		{
			CustTranspTable->tblInfo.Width = 64;
			CustTranspTable->tblInfo.Height = 48;

			//
			CustTranspTable->ColumnHeader = Rect(0,0, CustTranspTable->tblInfo.Width, 24);
			CustTranspTable->RowHeader = Rect(0,0, 48, CustTranspTable->tblInfo.Height);

			CustTranspTable->BigFont = new Font(L"Verdana", 10);
			CustTranspTable->smallFont = new Font(L"Verdana", 8);

			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			//Color(50, 180, 170)
			CustTranspTable->HeaderColor = Color(70, 200, 190);
			CustTranspTable->HeaderTextColor = Color(240, 255, 250);
			CustTranspTable->GridColor = Color(80, 225, 210);

			CustTranspTable->tblInfo.Rows = 12;
			CustTranspTable->tblInfo.Columns = 16;

			HeaderElement hel, vel;
			GridElement gel;
			for (int i = 0; i < CustTranspTable->tblInfo.Columns; ++i)
				CustTranspTable->Consumers.push_back(hel);
			for (size_t p = 0; p < CustTranspTable->tblInfo.Rows; ++p)
				CustTranspTable->Providers.push_back(vel);

			for (size_t i = 0; i < CustTranspTable->tblInfo.Rows*CustTranspTable->tblInfo.Columns; ++i)
					CustTranspTable->Elements.push_back(gel);

			CustTranspTable->Update();
			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		}

		return (CustTranspTable != nullptr);

	case WM_NCDESTROY:
		if (CustTranspTable != nullptr)
		{
			DestroyWindow(CustTranspTable->hEdit);

			delete CustTranspTable;
		}
		break;

	case WM_SIZE:
		return CustTranspTable->OnSize(wParam, lParam);
	case WM_VSCROLL:
		CustTranspTable->OnVScroll(wParam, lParam);
		break;
	case WM_LBUTTONDOWN:
		//bool.. captured = true;
		//SetCapture(hWnd);
		return CustTranspTable->OnClick(wParam, lParam);
	case WM_LBUTTONUP:
		//ReleaseCapture();
	case NM_RELEASEDCAPTURE:
		//bool captured = false;
		break;
	case WM_LBUTTONDBLCLK:
		CustTranspTable->OnDoubleClick(wParam, lParam);
		break;

	case WM_COMMAND:
		if (HIWORD(wParam) == EN_SETFOCUS) {
			//std::stringstream st;
			//st << 
			SetWindowText(CustTranspTable->hEdit, L"0.15");
			GetWindowText(CustTranspTable->hEdit, buf, ARRAYSIZE(buf)); // Получаем строку
			return 0;
		}
		else if (HIWORD(wParam) == EN_KILLFOCUS)// При убирании фокуса от поля ввода
		{
			NumericEdit<double> nmEd;
			nmEd.SetHWNDs(CustTranspTable->hEdit);

			// Проверяем введёные данные
			if (!nmEd.InputCheck())
				SetWindowText(CustTranspTable->hEdit, buf);

			// Сбрасываем nmEd
			nmEd.SetHWNDs(nullptr);
			
			// Уничтожаем Edit
			DestroyWindow(CustTranspTable->hEdit);
			return 0;
		}

		break;
/*	case WM_MOUSEMOVE:
	{
		
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.dwHoverTime = HOVER_DEFAULT;
		tme.hwndTrack = hWnd;
		tme.dwFlags = TME_LEAVE | TME_HOVER;

		TrackMouseEvent(&tme);
		
		return CustTranspTable->OnClick(wParam, lParam);
	}*/
	case WM_PAINT:
		return CustTranspTable->OnPaint(wParam, lParam);
	case WM_ERASEBKGND:
		return FALSE;
	default:
		break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

void TransportTable::PlaceEdit(RectF & rc)
{
	Rect edrc(rc.X+1, rc.Y+rc.Height - 20, rc.Width-1, rc.Height);

	hEdit = CreateWindowEx(/*WS_EX_CLIENTEDGE*/0, L"EDIT", L"", WS_CHILD | WS_VISIBLE | ES_CENTER,
		edrc.X, edrc.Y, edrc.Width, 16,
		hWnd, NULL, GetModuleHandle(NULL), NULL);

	SetFocus(hEdit);
}

void TransportTable::SetupScrollbars()
{
	SCROLLINFO scrInfo;

	scrInfo.cbSize = sizeof(scrInfo);
	scrInfo.fMask = SIF_POS | SIF_PAGE | SIF_RANGE | SIF_TRACKPOS | SIF_DISABLENOSCROLL;

	scrInfo.nPos = sInfo.nVScrollPos;
	scrInfo.nPage = sInfo.nWindowRows;
	scrInfo.nMin = 0;
	scrInfo.nMax = tblInfo.Rows*tblInfo.Height - 1;

	::SetScrollInfo(hWnd, SB_VERT, &scrInfo, TRUE);

	scrInfo.nPos = sInfo.nHScrollPos;
	scrInfo.nPage = sInfo.nWindowColumns;
	scrInfo.nMax = tblInfo.Columns*tblInfo.Width - 1;

	::SetScrollInfo(hWnd, SB_HORZ, &scrInfo, TRUE);

	sInfo.nVScrollMax = tblInfo.Rows*tblInfo.Height - sInfo.nWindowRows;
	sInfo.nHScrollMax = tblInfo.Columns*tblInfo.Width - sInfo.nWindowColumns;
}

bool TransportTable::PinToBottomCorner()
{
	bool repos = false;

	if (sInfo.nHScrollPos + sInfo.nWindowColumns > tblInfo.Columns)
	{
		sInfo.nHScrollPos = tblInfo.Columns - sInfo.nWindowColumns;
		repos = true;
	}

	if (sInfo.nVScrollPos + sInfo.nWindowRows > tblInfo.Rows)
	{
		sInfo.nVScrollPos = tblInfo.Rows - sInfo.nWindowRows;
		repos = true;
	}

	return repos;
}

TransportTable::TransportTable(HWND hWnd){

	this->hWnd = hWnd;
	hEdit = nullptr;
	BigFont = smallFont = nullptr;

	ZeroMemory(&tblInfo, sizeof(TableInfo));
	tblInfo.SelCol = tblInfo.SelRow= -1;
}
/*
TransportTable::TransportTable(UINT Rows, UINT Cols, UINT ColsWidth, UINT RowsHeight)
{
	hWnd = hEdit1 = hEdit2 = nullptr;
	hMenu = nullptr;

	this->Rows = Rows;
	this->Columns = Cols;
	this->ColumnWidth = ColsWidth;
	this->RowHeight = RowsHeight;

	SelRow = SelCol = 0;
}
*/
ATOM TransportTable::Initialize()
{
	WNDCLASSEX wcex;

	memset(&wcex, 0, sizeof(WNDCLASSEX));
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.cbWndExtra = sizeof(TransportTable*); // Добавляем дополнительные данные для окна (привязываем указатель к окну)
	//										Thread safe | performs better | двойной щелчок
	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_CLASSDC | CS_PARENTDC | CS_DBLCLKS;
	wcex.lpfnWndProc = TranspTblWndProc;
	wcex.cbClsExtra = 0;
	wcex.hInstance = GetModuleHandle(NULL);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszClassName = className;

	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	return RegisterClassEx(&wcex);
}

BOOL TransportTable::Release()
{
	GdiplusShutdown(gdiplusToken);
	return UnregisterClass(className, GetModuleHandle(NULL));
}

HWND TransportTable::CreateTable(HINSTANCE hInst, HWND hWndParent, UINT x, UINT y, UINT Width, UINT Height, TableInfo &tInfo)
{
	this->hWnd = CreateWindowEx(WS_EX_CLIENTEDGE, className, L"TransportTableWindow",
								WS_VISIBLE | WS_CHILD, x, y, Width, Height,
								hWndParent, nullptr, GetModuleHandle(NULL), nullptr);
	return this->hWnd;
}

void TransportTable::Update()
{
	// Неправильный расчёт
	int rhw = RowHeader.X + RowHeader.Width;
	int eh = RowHeader.Height/2;
	size_t idx;

	for (size_t c = 0; c < Consumers.size(); ++c) {
		Consumers[c].ioRC.X = rhw+ColumnHeader.Width*(c+1);
		Consumers[c].ioRC.Y = ColumnHeader.Height;

		Consumers[c].ioRC.Width = ColumnHeader.Width;
		Consumers[c].ioRC.Height = ColumnHeader.Height;
	}

	for (size_t p = 0; p < Providers.size(); ++p) {
		Providers[p].ioRC.X = rhw;
		Providers[p].ioRC.Y = RowHeader.Height*(p+1)+eh;

		Providers[p].ioRC.Width = ColumnHeader.Width;
		Providers[p].ioRC.Height = 24;
	}

	for (size_t y = 0; y < tblInfo.Rows; ++y)
		for (size_t x = 0; x < tblInfo.Columns; ++x){
			idx = y * tblInfo.Columns + x;

			Elements[idx].costRC.X = rhw + ColumnHeader.Width + tblInfo.Width*x + tblInfo.Width/2;
			Elements[idx].costRC.Y = RowHeader.Height*y + tblInfo.Height;

			Elements[idx].costRC.Width = tblInfo.Width/2;
			Elements[idx].costRC.Height = tblInfo.Height/2;
		}

	//UpdateWindow(this->hWnd);
}

TransportTable::~TransportTable(){

	if (BigFont != nullptr)
		delete BigFont;

	if (smallFont != nullptr)
		delete smallFont;
};

bool TransportTable::SetColumnHeaders(wchar_t ** cName, int Num)
{

	return false;
}

bool TransportTable::SetRowHeaders(wchar_t ** rName, int Num)
{

	return false;
}

bool TransportTable::GetArrays(TransportData *A, TransportData *B, TransportData *Costs, TransportData *Transports)
{
	if(tblInfo.Rows == 0 && tblInfo.Columns == 0)
		return false;



	return true;
}

bool TransportTable::AddRows(UINT num)
{
	if(num == 0)
		return false;
	
	// Array of elements
	tblInfo.Rows += num;

	return true;
}
