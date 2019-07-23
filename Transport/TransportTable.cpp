#include "TransportTable.hpp"

//using namespace Gdiplus;

LPCWSTR TransportTable::className = L"TransportTable";
wchar_t TransportTable::buf[48] = { 0 };

// GDI+
GdiplusStartupInput TransportTable::gdiplusStartupInput;
ULONG_PTR TransportTable::gdiplusToken;

// Сохранение и извлечение указателя на объект TransportTable
TransportTable *TransportTable::GetTransportCtrl(HWND hWnd)
{
	return reinterpret_cast<TransportTable*>(GetWindowLong(hWnd, 0));
}

void TransportTable::SetTransportCtrl(HWND hWnd, TransportTable *TranspTable)
{
	SetWindowLong(hWnd, 0, reinterpret_cast<LONG>(TranspTable));
}

void TransportTable::HollowLine(Graphics *gp, Pen *pen1, Pen *pen2, Point *p1, Point *p2, int dir) {

	if (gp != nullptr && pen1 != nullptr && pen2 != nullptr && p1 != nullptr && p2 != nullptr)
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

void TransportTable::DrawElements(Graphics *gp, Rect &Borders)
{
	Rect rc;

	SolidBrush whiteText(Color(255, 255, 255)), blackText(Color(0,0,0));
	SolidBrush Selected(Color(50, 100,255,135));
	// Форматирование строки
	StringFormat sf(StringFormatFlagsLineLimit);
	sf.SetAlignment(StringAlignmentCenter);
	sf.SetTrimming(StringTrimmingCharacter);
	sf.SetLineAlignment(StringAlignmentCenter);

	size_t size = 0, idx, idy;

	for (int c = 0; c < sInfo.nWindowColumns; ++c) {
		idx = c + sInfo.nHScrollPos;
		size = wcslen(Consumers[idx].hVal);
		gp->DrawString(Consumers[idx].hVal, size, BigFont, ConsRC[c], &sf, &blackText);//c
	}

	for (int r = 0; r < sInfo.nWindowRows; ++r) {
		idx = r + sInfo.nVScrollPos;
		size = wcslen(Providers[idx].hVal);
		gp->DrawString(Providers[idx].hVal, size, BigFont, ProvRC[r], &sf, &blackText);//r
	}

	if (tblInfo.SelRow >= 0 && tblInfo.SelCol >= 0) {
		rc.X = RowHeader.Width + (tblInfo.SelCol - sInfo.nHScrollPos + 1)*tblInfo.Width;
		rc.Y = Borders.Y;
		rc.Height = Borders.Height;
		rc.Width = tblInfo.Width;

		gp->FillRectangle(&Selected, rc);

		rc.X = Borders.X;
		rc.Y = ColumnHeader.Height * 2 + (tblInfo.SelRow - sInfo.nVScrollPos)*tblInfo.Height;
		rc.Height = tblInfo.Height;
		rc.Width = Borders.Width;

		gp->FillRectangle(&Selected, rc);
	}

	for (size_t y = 0; y < sInfo.nWindowRows; ++y)
		for (size_t x = 0; x < sInfo.nWindowColumns; ++x) {
			idx = (y + sInfo.nVScrollPos) * tblInfo.Columns + (x + sInfo.nHScrollPos);
			idy =  y * sInfo.nWindowColumns + x;

			size = wcslen(Elements[idx].cVal);
			gp->DrawString(Elements[idx].cVal, size, BigFont, CostRC[idy], &sf, &blackText);

			size = wcslen(Elements[idx].tVal);
			gp->DrawString(Elements[idx].tVal, size, BigFont, TranspRC[idy], &sf, &blackText);
		}
}

void TransportTable::DrawGrid(Graphics *gp, Pen *pen, Rect &Borders) {
	Point pt1X, pt2X, pt1Y, pt2Y;
	Rect rate;

	if (gp != nullptr && pen != nullptr)
	{
		rate.Width = tblInfo.Width / 2;
		rate.Height = tblInfo.Height / 2;

		for (int x = 0; x < sInfo.nWindowColumns + 1; ++x)
		{
			pt1X.X = pt2X.X = Borders.X + x*tblInfo.Width;
			pt1X.Y = ColumnHeader.Height;
			pt2X.Y = Borders.Height;

			gp->DrawLine(pen, pt1X, pt2X);
		}

		for (int y = 0; y < sInfo.nWindowRows + 1; ++y)
		{
			pt1Y.Y = pt2Y.Y = Borders.Y + y*tblInfo.Height;
			pt1Y.X = RowHeader.Width;
			pt2Y.X = Borders.Width;

			gp->DrawLine(pen, pt1Y, pt2Y);
		}

		for (int x = 0; x < sInfo.nWindowColumns; ++x)
			for (int y = 0; y < sInfo.nWindowRows; ++y)
			{
				rate.X = Borders.X + rate.Width*x + rate.Width*(x+1);
				rate.Y = Borders.Y + rate.Height*y + rate.Height*y;
				gp->DrawRectangle(pen, rate);
			}
	}
}

void TransportTable::DrawHeader(Graphics *gp, Rect &Borders)
{
	Point pt1, pt2;
	Rect HeaderRC;
	//
	pt1.Y = 0;
	pt2.Y = ColumnHeader.Height;
	//
	SolidBrush HeaderBrush(this->HeaderColor);
	//
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

		for (int c = 0; c < sInfo.nWindowColumns + 1; ++c)
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

		for (int r = 0; r < sInfo.nWindowRows + 1; ++r)
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
	// Устанавливаем кисть для текста
	SolidBrush penText(HeaderTextColor);
	// Форматирование строки
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

		for (int c = 0; c < sInfo.nWindowColumns; ++c)
		{
			trc.X = RowHeader.Width + ColumnHeader.Width + c*ColumnHeader.Width;

			swprintf(buf, L"%d", c + sInfo.nHScrollPos + 1);
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

		for (int r = 0; r < sInfo.nWindowRows; ++r)
		{
			trc.Y = RowHeader.Height + r*RowHeader.Height;

			swprintf(buf, L"%d", r + sInfo.nVScrollPos + 1);
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

LRESULT TransportTable::OnPaint(WPARAM wParam, LPARAM lParam)
{
	//
	HDC hdc;
	PAINTSTRUCT ps;
	//
	RECT rc;
	// Цвет выделения
	SolidBrush sel(Color(150, 0, 160, 255));

	hdc = BeginPaint(this->hWnd, &ps);

	//ps.rcPaint;
	GetClientRect(this->hWnd, &rc);

    //rc.right -= 20;

	Rect drc, GridBorders;
	memcpy(&drc, &rc, sizeof(RECT));
	
	dirtRC.X = max(ps.rcPaint.left, RowHeader.Width + tblInfo.Width);
	dirtRC.Y = max(ps.rcPaint.top, ColumnHeader.Height * 2);
	dirtRC.Width = ps.rcPaint.right - ps.rcPaint.left;
	dirtRC.Height = ps.rcPaint.bottom - ps.rcPaint.top;

	dInfo.RowMin = (dirtRC.X - ColumnHeader.Height*2 + tblInfo.Height*sInfo.nVScrollPos -
					(tblInfo.Height - 1)) / tblInfo.Height;

	dInfo.RowMax = (dirtRC.Height - ColumnHeader.Height*2 + tblInfo.Height*sInfo.nVScrollPos +
					(tblInfo.Height - 1)) / tblInfo.Height;

	if (dInfo.RowMax > tblInfo.Rows)
		dInfo.RowMax = tblInfo.Rows;

	Graphics ttcd(hdc);

	// Отображение с помощью двойной буферизации
	Bitmap BackImg(drc.Width, drc.Height, &ttcd);
	Graphics backBuffer(&BackImg);

	SolidBrush bckgnd(Color(255, 255, 255));
	Pen GridPen(this->GridColor);

	backBuffer.FillRectangle(&bckgnd, drc);

	GridBorders.X = dirtRC.X;
	GridBorders.Y = dirtRC.Y;
	GridBorders.Width = drc.Width;
	GridBorders.Height = drc.Height;

	DrawGrid(&backBuffer, &GridPen, GridBorders);
	DrawHeader(&backBuffer, drc);
	DrawHeaderText(&backBuffer);
	/**/
	DrawElements(&backBuffer, drc);
	/**/

	// Показываем результат отрисовки
	ttcd.DrawImage(&BackImg, drc);
	
	EndPaint(this->hWnd, &ps);

	return 0;
}

BOOL TransportTable::OnVScroll(WPARAM wParam, LPARAM lParam)
{
	BOOL bResult = TRUE;
	SCROLLINFO scrInfo;
	LONG oldY, newY;
	RECT wndRC;

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
		GetClientRect(this->hWnd, &wndRC);
		wndRC.top += ColumnHeader.Height*2+1;
		sInfo.nVScrollPos = scrInfo.nPos;

		newY = tblInfo.Height*(oldY - scrInfo.nPos);

		bResult = ScrollWindowEx(this->hWnd, 0, newY,
								 NULL, &wndRC, NULL, NULL, SW_ERASE | SW_INVALIDATE);

		InvalidateRect(this->hWnd, NULL, FALSE);

		if (this->hEdit != nullptr){
			EditRC.Y += newY;

			SetWindowPos(this->hEdit, NULL, EditRC.X, EditRC.Y,
											EditRC.Width, EditRC.Height, SWP_NOSIZE | SWP_NOREDRAW);
		}

		SetupScrollbars();

		//ScrollBar length/width - 18px
		//Update();
	}

	return bResult;
}

BOOL TransportTable::OnHScroll(WPARAM wParam, LPARAM lParam)
{
	BOOL bResult = TRUE;/////////////////////////////////////////////////////////////////////////////////////
	SCROLLINFO scrInfo;
	LONG oldX, newX;
	RECT wndRC;

	scrInfo.cbSize = sizeof(scrInfo);
	scrInfo.fMask = SIF_ALL;

	::GetScrollInfo(hWnd, SB_HORZ, &scrInfo);
	oldX = scrInfo.nPos;

	switch (LOWORD(wParam))
	{
		// Пользователь нажал клавишу Home
	case SB_LEFT:
		scrInfo.nPos = scrInfo.nMin;
		break;
		// Пользователь нажал клавишу End
	case SB_RIGHT:
		scrInfo.nPos = scrInfo.nMax;
		break;
		// Пользователь нажал левую стрелку
	case SB_LINELEFT:
		--scrInfo.nPos;
		break;
		// Пользователь нажал правую стрелку
	case SB_LINERIGHT:
		++scrInfo.nPos;
		break;
		// Пользователь нажал на место над движком
	case SB_PAGELEFT:
		scrInfo.nPos -= scrInfo.nPage;
		break;
		// Пользователь нажал на место под движком
	case SB_PAGERIGHT:
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
	SetScrollInfo(this->hWnd, SB_HORZ, &scrInfo, TRUE);
	GetScrollInfo(this->hWnd, SB_HORZ, &scrInfo);

	// Если позиция изменилась, листаем окно и обновляем его
	if (scrInfo.nPos != oldX)
	{
		GetClientRect(this->hWnd, &wndRC);
		wndRC.left += RowHeader.Width + ColumnHeader.Width + 1;
		sInfo.nHScrollPos = scrInfo.nPos;

		newX = tblInfo.Width*(oldX - scrInfo.nPos);

		bResult = ScrollWindowEx(this->hWnd, newX, 0,
								 NULL, &wndRC, NULL, NULL, SW_ERASE | SW_INVALIDATE);

		InvalidateRect(this->hWnd, NULL, FALSE);

		if (this->hEdit != nullptr){
			EditRC.X += newX;

			SetWindowPos(this->hEdit, NULL, EditRC.X, EditRC.Y,
						 EditRC.Width, EditRC.Height, SWP_NOSIZE | SWP_NOREDRAW);
		}

		SetupScrollbars();

		//ScrollBar length/width - 18px
		//Update();
	}

	return bResult;
}

LRESULT TransportTable::OnSize(WPARAM wParam, LPARAM lParam)
{
	RECT rc;

	GetWindowRect(hWnd, &rc);

	Update();

	if (PinToBottomCorner())
		UpdateWindow(hWnd);

	SetupScrollbars();

	return LRESULT();
}

LRESULT TransportTable::OnClick(WPARAM wParam, LPARAM lParam)
{
	// Координаты мыши
	POINTS pts = MAKEPOINTS(lParam);

	SetFocus(this->hWnd);
	// Координаты выделения
	tblInfo.SelCol = tblInfo.SelRow = -1;
	dInfo.ColCurSel = dInfo.RowCurSel = -1;
	// Поиск пересечения с прямоугольниками
	for (int y = 0; y < sInfo.nWindowRows; ++y) {

		if (ProvRC[y].Contains(pts.x, pts.y)){
			dInfo.RowCurSel = y;
			tblInfo.SelRow = y + sInfo.nVScrollPos;
		}

		for (int x = 0; x < sInfo.nWindowColumns; ++x)
		{
			if (ConsRC[x].Contains(pts.x, pts.y)){
				dInfo.ColCurSel = x;
				tblInfo.SelCol = x + sInfo.nHScrollPos;
			}

			if (CostRC[y*sInfo.nWindowColumns + x].Contains(pts.x, pts.y)) {
				dInfo.ColCurSel = x;
				dInfo.RowCurSel = y;

				tblInfo.SelCol = x + sInfo.nHScrollPos; 
				tblInfo.SelRow = y + sInfo.nVScrollPos;
			}
		}
	}

	InvalidateRect(this->hWnd, NULL, FALSE);
	//SendMessage(this->hWnd, WM_PAINT, 0, 0);

	return 0;
}

LRESULT TransportTable::OnDoubleClick(WPARAM wParam, LPARAM lParam)
{
	// Если был выбран хотя-бы один из элементов, то размещаем поле редактирования
	if (!(dInfo.RowCurSel < 0) && dInfo.ColCurSel < 0) {
		PlaceEdit(ProvRC[dInfo.RowCurSel], Providers[tblInfo.SelRow].hVal);
	};

	if (!(dInfo.ColCurSel < 0) && dInfo.RowCurSel < 0) {
		PlaceEdit(ConsRC[dInfo.ColCurSel], Consumers[tblInfo.SelCol].hVal);
	};

	if (!(dInfo.ColCurSel < 0) && !(dInfo.RowCurSel < 0)) {
		PlaceEdit(CostRC[dInfo.RowCurSel*sInfo.nWindowColumns + dInfo.ColCurSel],
				  Elements[tblInfo.SelRow*tblInfo.Columns + tblInfo.SelCol].cVal);
	};

	return 0;
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
			CustTranspTable->tblInfo.Width = 78;
			CustTranspTable->tblInfo.Height = 48;

			//
			CustTranspTable->ColumnHeader = Rect(0,0, CustTranspTable->tblInfo.Width, 24);
			CustTranspTable->RowHeader = Rect(0,0, 48, CustTranspTable->tblInfo.Height);

			CustTranspTable->BigFont = new Font(L"Verdana", 10);
			CustTranspTable->smallFont = new Font(L"Verdana", 8);

			CustTranspTable->hFnt = CreateFont(
				-12, 0, 0, 0, FW_NORMAL, 0, 0, 0, // -12 - абсолютный размер в пикселях
				DEFAULT_CHARSET, OUT_TT_ONLY_PRECIS, CLIP_DEFAULT_PRECIS,
				CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Verdana");
			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			//Color(50, 180, 170)
			CustTranspTable->HeaderColor = Color(70, 200, 190);
			CustTranspTable->HeaderTextColor = Color(240, 255, 250);
			CustTranspTable->GridColor = Color(80, 225, 210);
			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		}

		return (CustTranspTable != nullptr);

	case WM_NCDESTROY:
		if (CustTranspTable != nullptr)
		{
			DestroyWindow(CustTranspTable->hEdit);
			DeleteObject(CustTranspTable->hFnt);

			delete CustTranspTable;
		}
		break;

	case WM_SIZE:
		return CustTranspTable->OnSize(wParam, lParam);
	case WM_VSCROLL:
		CustTranspTable->OnVScroll(wParam, lParam);
		break;
	case WM_HSCROLL:
		CustTranspTable->OnHScroll(wParam, lParam);
		break;
		//WM_MOVING
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
		return CustTranspTable->OnDoubleClick(wParam, lParam);
	case WM_COMMAND:
		if (HIWORD(wParam) == EN_SETFOCUS) {
			GetWindowText(CustTranspTable->hEdit, buf, ARRAYSIZE(buf)); // Получаем строку
			return 0;
		}
		else if (HIWORD(wParam) == EN_KILLFOCUS)// При убирании фокуса от поля ввода
		{
			NumericEdit<double> nmEd;
			nmEd.SetHWNDs(CustTranspTable->hEdit);
			nmEd.SetMinMaxValues(0, std::numeric_limits<double>::infinity());

			// Проверяем введёные данные
			if (!nmEd.InputCheck(true))
				SetWindowText(CustTranspTable->hEdit, buf);

			// Сбрасываем nmEd
			nmEd.SetHWNDs(nullptr);

			// Обновляем данные транспортной таблицы
			GetWindowText(CustTranspTable->hEdit, buf, sizeof(wchar_t)*48);

			if (!(CustTranspTable->tblInfo.SelRow < 0) &&
				  CustTranspTable->tblInfo.SelCol < 0) {

				memcpy(CustTranspTable->Providers[CustTranspTable->tblInfo.SelRow].hVal,
					   buf, sizeof(wchar_t) * 48);
			};

			if (!(CustTranspTable->tblInfo.SelCol < 0) &&
				  CustTranspTable->tblInfo.SelRow < 0) {

				memcpy(CustTranspTable->Consumers[CustTranspTable->tblInfo.SelCol].hVal,
					   buf, sizeof(wchar_t) * 48);;
			};

			if (!(CustTranspTable->tblInfo.SelCol < 0) &&
				!(CustTranspTable->tblInfo.SelRow < 0)) {

				memcpy(CustTranspTable->Elements[CustTranspTable->tblInfo.SelRow *
					   CustTranspTable->tblInfo.Columns +
					   CustTranspTable->tblInfo.SelCol].cVal,
					   buf, sizeof(wchar_t) * 48);
			}

			CustTranspTable->Update();
			
			// Уничтожаем Edit
			DestroyWindow(CustTranspTable->hEdit);
			CustTranspTable->hEdit = nullptr;

			return 0;
		}

		break;
	case WM_PAINT:
		return CustTranspTable->OnPaint(wParam, lParam);

	case WM_ERASEBKGND:
		return FALSE;
	default:
		break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

void TransportTable::PlaceEdit(RectF &rc, wchar_t *textVal)
{
	EditRC = Rect(rc.X+1, rc.Y+rc.Height - 20, rc.Width-1, 16);

	if (textVal != nullptr && hFnt != nullptr){
		hEdit = CreateWindowEx(0, L"EDIT", textVal,
				WS_CHILD | WS_VISIBLE | ES_CENTER | ES_AUTOHSCROLL,
				EditRC.X, EditRC.Y, EditRC.Width, EditRC.Height,
				hWnd, NULL, GetModuleHandle(NULL), NULL);

		SendMessage(hEdit, WM_SETFONT, reinterpret_cast<WPARAM>(hFnt), MAKELONG(TRUE, 0));
		SetFocus(hEdit);
	}
}

void TransportTable::SetupScrollbars()
{
	SCROLLINFO scrInfo;

	scrInfo.cbSize = sizeof(scrInfo);
	scrInfo.fMask = SIF_POS | SIF_PAGE | SIF_RANGE | SIF_TRACKPOS /*| SIF_DISABLENOSCROLL*/;

	scrInfo.nPos = sInfo.nVScrollPos;
	scrInfo.nPage = sInfo.nWindowRows;
	scrInfo.nMin = 0;
	scrInfo.nMax = tblInfo.Rows - 1;

	::SetScrollInfo(hWnd, SB_VERT, &scrInfo, TRUE);

	scrInfo.nPos = sInfo.nHScrollPos;
	scrInfo.nPage = sInfo.nWindowColumns;
	scrInfo.nMax = tblInfo.Columns - 1;

	::SetScrollInfo(hWnd, SB_HORZ, &scrInfo, TRUE);

	sInfo.nVScrollMax = tblInfo.Rows - sInfo.nWindowRows;
	sInfo.nHScrollMax = tblInfo.Columns - sInfo.nWindowColumns;
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
	hFnt = nullptr;
	BigFont = smallFont = nullptr;

	ZeroMemory(&tblInfo, sizeof(TableInfo));
	tblInfo.SelCol = tblInfo.SelRow= -1;
}

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

bool TransportTable::AddRow()
{
	return false;
}

bool TransportTable::AddColumn()
{
	return false;
}

void TransportTable::DeleteTable()
{
	Providers.clear();
	Consumers.clear();
	Elements.clear();

	tblInfo.Rows = tblInfo.Columns = 0;
	tblInfo.SelCol = tblInfo.SelRow = -1;
}

void TransportTable::Update()
{
	// Неправильный расчёт

	int rhw = RowHeader.X + RowHeader.Width;
	int eh = RowHeader.Height/2;
	size_t idx;
	RECT rc;

	GetWindowRect(this->hWnd, &rc);
	sInfo.nWindowRows = min((rc.bottom - rc.top - RowHeader.Height) / tblInfo.Height, tblInfo.Rows);
	sInfo.nWindowColumns = min((rc.right - rc.left - RowHeader.Width - ColumnHeader.Width) / tblInfo.Width, tblInfo.Columns);

    if (sInfo.nWindowColumns > 0) {
        ConsRC.resize(sInfo.nWindowColumns);

        for (size_t c = 0; c < sInfo.nWindowColumns; ++c) {
            ConsRC[c].X = rhw + ColumnHeader.Width*(c + 1);
            ConsRC[c].Y = ColumnHeader.Height;

            ConsRC[c].Width = ColumnHeader.Width;
            ConsRC[c].Height = ColumnHeader.Height;
        }
    }
    else {
        ConsRC.resize(0);
    }

    if (sInfo.nWindowColumns > 0) {
        ProvRC.resize(sInfo.nWindowRows);

        for (size_t p = 0; p < sInfo.nWindowRows; ++p) {
            ProvRC[p].X = rhw;
            ProvRC[p].Y = RowHeader.Height*(p + 1) + eh;

            ProvRC[p].Width = ColumnHeader.Width;
            ProvRC[p].Height = 24;
        }
    }
    else {
        ProvRC.resize(0);
    }

    if (sInfo.nWindowColumns > 0 && sInfo.nWindowColumns > 0) {
        TranspRC.resize(sInfo.nWindowColumns*sInfo.nWindowRows);
        CostRC.resize(sInfo.nWindowColumns*sInfo.nWindowRows);

        for (size_t y = 0; y < sInfo.nWindowRows; ++y) {
            for (size_t x = 0; x < sInfo.nWindowColumns; ++x) {
                idx = y * sInfo.nWindowColumns + x;

                TranspRC[idx].X = rhw + ColumnHeader.Width + tblInfo.Width*x;
                TranspRC[idx].Y = RowHeader.Height*y + tblInfo.Height*1.5;

                TranspRC[idx].Width = tblInfo.Width;
                TranspRC[idx].Height = tblInfo.Height / 2;

                CostRC[idx].X = rhw + ColumnHeader.Width + tblInfo.Width*x + tblInfo.Width / 2;
                CostRC[idx].Y = RowHeader.Height*y + tblInfo.Height;

                CostRC[idx].Width = tblInfo.Width / 2;
                CostRC[idx].Height = tblInfo.Height / 2;
            }
        }
    }
    else {
        TranspRC.resize(0);
        CostRC.resize(0);
    }

	PinToBottomCorner();

	InvalidateRect(this->hWnd, NULL, FALSE);

	SetupScrollbars();
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

TableInfo TransportTable::GetTableInfo()
{
	return tblInfo;
}

bool TransportTable::GetData(TransportData *&Prov, TransportData *&Cons, TransportData *&Costs, TransportData *&Transports, TableInfo &info)
{
	if(tblInfo.Rows == 0 || tblInfo.Columns == 0)
		return false;

	Prov = new TransportData[tblInfo.Rows];
	Cons = new TransportData[tblInfo.Columns];
	Costs = new TransportData[tblInfo.Rows*tblInfo.Columns];
	Transports = new TransportData[tblInfo.Rows*tblInfo.Columns];

	info = tblInfo;

	ZeroMemory(Prov, sizeof(TransportData)*tblInfo.Rows);
	ZeroMemory(Cons, sizeof(TransportData)*tblInfo.Columns);
	ZeroMemory(Costs, sizeof(TransportData)*tblInfo.Rows*tblInfo.Columns);
	ZeroMemory(Transports, sizeof(TransportData)*tblInfo.Rows*tblInfo.Columns);

	size_t idx;
	TransportData tempData = 0;

	for (size_t y = 0; y < tblInfo.Rows; ++y) {
		Prov[y] = _wtof(Providers[y].hVal);
	}
		
	for (size_t x = 0; x < tblInfo.Columns; ++x) {
		Cons[x] = _wtof(Consumers[x].hVal);
	}

	for (size_t y = 0; y < tblInfo.Rows; ++y){
		for (size_t x = 0; x < tblInfo.Columns; ++x) {
			idx = y*tblInfo.Columns + x;

			Costs[idx] = _wtof(Elements[idx].cVal);
			Transports[idx] = _wtof(Elements[idx].tVal);
		}
	}

	return true;
}

bool TransportTable::SetData(TransportData *Transports)
{
	if (Transports == nullptr || tblInfo.Rows < 2 || tblInfo.Columns < 2) {
		return false;
	}

	size_t idx;

	for (size_t y = 0; y < tblInfo.Rows; ++y) {
		for (size_t x = 0; x < tblInfo.Columns; ++x) {
			idx = y*tblInfo.Columns + x;

			swprintf(Elements[idx].tVal, L"%g", Transports[idx]);
		}
	}

	return true;
}

bool TransportTable::Open(std::wstring fileName)
{
	TransportData *Prov = nullptr, *Cons = nullptr, *Costs = nullptr, *Transports = nullptr;
	size_t idx;

	std::ifstream fs(fileName, std::ios::binary);

	if (fs.is_open()) {
		fs.read((char*)&tblInfo, sizeof(TableInfo));

		if (tblInfo.Rows > 0) {
			Providers.resize(tblInfo.Rows);
			Prov = new TransportData[tblInfo.Rows];

			fs.read((char*)Prov, sizeof(TransportData)*tblInfo.Rows);

			for (size_t p = 0; p < tblInfo.Rows; ++p)
				swprintf(Providers[p].hVal, L"%g", Prov[p]);

			delete[] Prov;
		}

		if (tblInfo.Columns > 0) {
			Consumers.resize(tblInfo.Columns);
			Cons = new TransportData[tblInfo.Columns];

			fs.read((char*)Cons, sizeof(TransportData)*tblInfo.Columns);

			for (size_t c = 0; c < tblInfo.Columns; ++c)
				swprintf(Consumers[c].hVal, L"%g", Cons[c]);

			delete[] Cons;
		}

		if (tblInfo.Rows > 0 && tblInfo.Columns > 0) {
			Elements.resize(tblInfo.Rows*tblInfo.Columns);
			Costs = new TransportData[tblInfo.Rows*tblInfo.Columns];
			Transports = new TransportData[tblInfo.Rows*tblInfo.Columns];

			fs.read((char*)Costs, sizeof(TransportData)*tblInfo.Rows*tblInfo.Columns);
			fs.read((char*)Transports, sizeof(TransportData)*tblInfo.Rows*tblInfo.Columns);

			for (size_t y = 0; y < tblInfo.Rows; ++y) {
				for (size_t x = 0; x < tblInfo.Columns; ++x) {
					idx = y*tblInfo.Columns + x;

					swprintf(Elements[idx].cVal, L"%g", Costs[idx]);
					swprintf(Elements[idx].tVal, L"%g", Transports[idx]);
				}
			}

			delete[] Costs, Transports;
		}
	}
	else {
		MessageBox(hWnd, L"Не удалось открыть файл", L"Ошибка", MB_OK | MB_ICONERROR);
		fs.close();
		return false;
	}

	fs.close();

	return true;
}

bool TransportTable::Save(std::wstring fileName)
{
	TableInfo tInfo;
	TransportData *Prov = nullptr, *Cons = nullptr, *Costs = nullptr, *Transports = nullptr;

	std::ofstream fs(fileName, std::ios::binary);

	if (fs.is_open()) {
		if (GetData(Prov, Cons, Costs, Transports, tInfo)) {

			fs.write((char*)&tInfo, sizeof(TableInfo));

			if (tInfo.Rows > 0)
				fs.write((char*)Prov, sizeof(TransportData)*tInfo.Rows);

			if (tInfo.Columns > 0)
				fs.write((char*)Cons, sizeof(TransportData)*tInfo.Columns);

			if (tInfo.Rows > 0 && tInfo.Columns > 0) {
				fs.write((char*)Costs, sizeof(TransportData)*tInfo.Rows*tInfo.Columns);
				fs.write((char*)Transports, sizeof(TransportData)*tInfo.Rows*tInfo.Columns);
			}
		}
		else {
			MessageBox(hWnd, L"Не удалось сохранить файл", L"Ошибка", MB_OK | MB_ICONERROR);
			fs.close();
			return false;
		}

		fs.close();
	}
	else {
		return false;
	}

	return true;
}

bool TransportTable::AddRows(UINT num)
{
	wchar_t null[] = L"0";
	size_t newSize;

	HeaderElement headEl;
	GridElement gridEl;
	std::vector<GridElement>::iterator itGrd;
	std::vector<HeaderElement>::iterator itHel;
	// 
	if(num == 0)
		return false;
	//
	newSize = tblInfo.Rows + num;

	ZeroMemory(headEl.hVal, sizeof headEl.hVal);
	swprintf(headEl.hVal, null);
	swprintf(gridEl.tVal, null);
	swprintf(gridEl.cVal, null);
	// Меняем размер
	Providers.resize(newSize);

	//if (tblInfo.Columns > 0)
	//	Elements.resize(newSize*tblInfo.Columns);

	for (itHel = Providers.begin() + tblInfo.Rows;
		 itHel != Providers.end(); ++itHel) {

		*itHel = headEl;
	}

	for (size_t y = tblInfo.Rows; y < newSize; ++y) {
		for (size_t x = 0; x < tblInfo.Columns; ++x) {

			itGrd = Elements.begin() + y*tblInfo.Columns + x;
			Elements.insert(itGrd, gridEl);
		}
	}

	tblInfo.Rows = newSize;

	return true;
}

bool TransportTable::AddColumns(UINT num)
{
	wchar_t null[] = L"0";
	size_t newSize;

	HeaderElement headEl;
	GridElement gridEl;
	std::vector<GridElement>::iterator itGrd;
	std::vector<HeaderElement>::iterator itHel;
	// 
	if (num == 0)
		return false;
	//
	newSize = tblInfo.Columns + num;

	ZeroMemory(headEl.hVal, sizeof headEl.hVal);
	swprintf(headEl.hVal, null);
	swprintf(gridEl.tVal, null);
	swprintf(gridEl.cVal, null);
	// Меняем размер
	Consumers.resize(newSize);

	//if(tblInfo.Rows > 0)
	//	Elements.resize(newSize*tblInfo.Rows);

	for (itHel = Consumers.begin() + tblInfo.Columns;
		 itHel != Consumers.end(); ++itHel) {

		*itHel = headEl;
	}

	for (size_t y = 0; y < tblInfo.Rows; ++y) {
		for (size_t x = tblInfo.Columns; x < newSize; ++x) {

			itGrd = Elements.begin() + y*newSize + x;
			Elements.insert(itGrd, gridEl);
		}
	}

	tblInfo.Columns = newSize;

	return true;
}

bool TransportTable::SetRowHeader(UINT row, wchar_t *hVal)
{
	size_t len = 0;

	if (hVal != nullptr && tblInfo.Rows > 0) {
		len = wcslen(hVal);

		if(len < 48)
			memcpy(Providers[row].hVal, hVal, sizeof(wchar_t)*len);
		else
			memcpy(Providers[row].hVal, hVal, sizeof Providers[row].hVal);

		return true;
	}
	return false;
}

bool TransportTable::SetColumnHeader(UINT col, wchar_t *hVal)
{
	size_t len = 0;

	if (hVal != nullptr && tblInfo.Columns > 0) {
		len = wcslen(hVal);

		if (len < 48)
			memcpy(Consumers[col].hVal, hVal, sizeof(wchar_t)*len);
		else
			memcpy(Consumers[col].hVal, hVal, sizeof Consumers[col].hVal);

		return true;
	}
	return false;
}

bool TransportTable::DeleteRows(UINT from, UINT to)
{
	std::vector<HeaderElement>::iterator itHDelFrom, itHDelTo;
	std::vector<GridElement>::iterator itElDelFrom, itElDelTo;

	if (from == to || tblInfo.Rows < 2 ||
		from >= tblInfo.Rows || to >= tblInfo.Rows)
		return false;

	itHDelFrom = Providers.begin() + from;
	itHDelTo = Providers.begin() + to;

	if (from < to) {
		tblInfo.Rows -= to - from;
		Providers.erase(itHDelFrom, itHDelTo);
	}
	else {
		tblInfo.Rows -= from - to;
		Providers.erase(itHDelTo, itHDelFrom);
	}

	itElDelFrom = Elements.begin() + from*tblInfo.Columns;
	itElDelTo = Elements.begin() + to*tblInfo.Columns;

	Elements.erase(itElDelFrom, itElDelTo);

	return true;
}

bool TransportTable::DeleteColumns(UINT from, UINT to)
{
	std::vector<HeaderElement>::iterator itHDelFrom, itHDelTo;
	std::vector<GridElement>::iterator itElDelFrom, itElDelTo;

	if (from == to || tblInfo.Columns < 2 ||
		from >= tblInfo.Columns || to >= tblInfo.Columns)
		return false;

	itHDelFrom = Consumers.begin() + from;
	itHDelTo = Consumers.begin() + to;

	if (from < to) {
		tblInfo.Columns -= to - from;
		Consumers.erase(itHDelFrom, itHDelTo);
	}
	else {
		tblInfo.Columns -= from - to;
		Consumers.erase(itHDelTo, itHDelFrom);
	}
	// need old rows and cols
	for (int r = 0; r < tblInfo.Rows; ++r) {
		itElDelFrom = Elements.begin() + from + r*tblInfo.Columns;
		itElDelTo = Elements.begin() + to + r*tblInfo.Columns;

		Elements.erase(itElDelFrom, itElDelTo);
	}

	return true;
}