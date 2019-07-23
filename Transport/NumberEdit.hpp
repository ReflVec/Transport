#pragma once

#include "stdafx.h"
#include "resource.h"

template <typename numType>
class NumericEdit {
	HWND hEdit, hUpDown;
	numType minValue, maxValue;
public:
	NumericEdit();
	~NumericEdit();

	HWND CreateNumericEdit(HINSTANCE hInst, HMENU hMenu);
	void SetHWNDs(HWND hEdit, HWND hUpDown = nullptr);
//	void SetDigitsLimit();
	void SetMinMaxValues(numType min, numType max);

	bool InputCheck(bool minmax);

/*	static LRESULT CALLBACK NumEditProc(HWND hWnd, UINT uMsg, // Subclassing
										WPARAM wParam, LPARAM lParam,
										UINT_PTR uIdSubclass, DWORD_PTR dwRefData);*/
};

template <typename numType>
NumericEdit<numType>::NumericEdit()
{
	hEdit = hUpDown = nullptr;

	minValue = (std::numeric_limits<numType>::min)();
	maxValue = (std::numeric_limits<numType>::max)();
}

template <typename numType>
NumericEdit<numType>::~NumericEdit()
{
	if (hEdit != nullptr)
		DestroyWindow(hEdit);

	if (hUpDown != nullptr)
		DestroyWindow(hUpDown);
}

template<typename numType>
inline HWND NumericEdit<numType>::CreateNumericEdit(HINSTANCE hInst, HMENU hMenu)
{
	return HWND();
}

template <typename numType>
void NumericEdit<numType>::SetHWNDs(HWND hEdit, HWND hUpDown)
{
	this->hEdit = hEdit;
	this->hUpDown = hUpDown;
}

template <typename numType>
void NumericEdit<numType>::SetMinMaxValues(const numType min, const numType max)
{
	minValue = min; maxValue = max;

	if (hUpDown != nullptr)
		SendMessage(hUpDown, UDM_SETRANGE32, minValue, maxValue);
}

template <typename numType>
bool NumericEdit<numType>::InputCheck(bool minmax)
{
	int length = 0;
	numType val = -std::numeric_limits<numType>::infinity();
	size_t err = std::string::npos;

	wchar_t wText[256] /*= nullptr*/;
	std::wstringstream sv;

	//length = GetWindowText(hEdit, nullptr, -1)+1;

	//if (length > 0)
	//	wText = new wchar_t[length];

	GetWindowText(hEdit, wText, 256);

	sv << wText;
	err = sv.str().find_first_not_of(L"0123456789+-.");
	sv >> val;

	if (hUpDown != nullptr)
	{
		SendMessage(hUpDown,
			UDM_GETRANGE32,
			reinterpret_cast<WPARAM>(&minValue), reinterpret_cast<LPARAM>(&maxValue));
	}

	if ((val < minValue || val > maxValue) && minmax)
	{
		val = max(val, minValue);
		val = min(val, maxValue);

		sv.clear();
		sv.str(L"");
		ZeroMemory(wText, sizeof(wchar_t) * 256/*length*/);

		sv << val;
		sv >> wText;

		SetWindowText(hEdit, wText);
	}
	else if (err != std::string::npos) {

		swprintf(wText, L"0");
		SetWindowText(hEdit, wText);
	}


	if (val == -std::numeric_limits<int>::infinity()) {
		//if (wText != nullptr)
		//	delete wText;

		return false;
	}

	//if (wText != nullptr)
	//	delete wText;

	return true;
}

template <typename numType>
NumericEdit<numType> *GetNumericEditCtrl(HWND hWnd)
{
	return static_cast<NumericEdit<numType>*>(reinterpret_cast<void*>(GetWindowLong(hWnd, 0)));
}

template <typename numType>
void SetNumericEditCtrl(HWND hWnd, NumericEdit<numType> *numEdit)
{
	SetWindowLong(hWnd, 0, reinterpret_cast<LONG>(numEdit));
}
/*
LRESULT CALLBACK NumericEdit::NumEditProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
int wmId, wmEvent;

wchar_t text[256];

switch (uMsg)
{
case EN_UPDATE:
//case WM_CHAR:
{
int minVal = 0, maxVal = 0, val = -1, sz;
std::wstringstream sv;

SendMessage(static_cast<HWND>(reinterpret_cast<void*>(dwRefData)),
UDM_GETRANGE32,
reinterpret_cast<WPARAM>(&minVal), reinterpret_cast<LPARAM>(&maxVal));

GetWindowText(hWnd, text, 256);

sv << text;
sv >> val;

if (val < minVal || val >= maxVal)
{
val = max(val, minVal);
val = min(val, maxVal);

sv.clear();
sv.str(L"");
ZeroMemory(text, sizeof text);

sv << val;
sv >> text;

SetWindowText(hWnd, text);
}

if (!((wParam >= '0' && wParam <= '9')
|| wParam == '.'
|| wParam == VK_RETURN
|| wParam == VK_DELETE
|| wParam == VK_BACK)) {

return 0;
}
break;
}
default:
break;
}
return DefSubclassProc(hWnd, uMsg, wParam, lParam);

}
*/