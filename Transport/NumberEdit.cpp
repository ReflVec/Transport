#include "NumberEdit.hpp"

NumericEdit::NumericEdit()
{
	hEdit = hUpDown = nullptr;
	
	minValue = (std::numeric_limits<int>::max)();
	maxValue = (std::numeric_limits<int>::min)();
}

NumericEdit::~NumericEdit()
{
	if (hEdit != nullptr)
		DestroyWindow(hEdit);

	if (hUpDown != nullptr)
		DestroyWindow(hUpDown);
}

void NumericEdit::SetHWNDs(HWND hEdit, HWND hUpDown)
{
	this->hEdit = hEdit;
	this->hUpDown = hUpDown;
}

void NumericEdit::SetMinMaxValues(int min, int max)
{
	minValue = min; maxValue = max;

	if (hUpDown != nullptr)
		SendMessage(hUpDown, UDM_SETRANGE32, minValue, maxValue);
}

bool NumericEdit::InputCheck()
{
	int length = 0, val = -INT_MAX; 
	size_t err;

	wchar_t wText[256] /*= nullptr*/;
	std::wstringstream sv;

	//length = GetWindowText(hEdit, nullptr, -1)+1;

	//if (length > 0)
	//	wText = new wchar_t[length];

	GetWindowText(hEdit, wText, 256);

	sv << wText;
	err = sv.str().find_first_not_of(L"0+");
	sv >> val;

	if (hUpDown != nullptr)
	{
		SendMessage(hUpDown,
					UDM_GETRANGE32,
					reinterpret_cast<WPARAM>(&minValue), reinterpret_cast<LPARAM>(&maxValue));
	}

	if (val < minValue || val > maxValue || err != std::string::npos)
	{
		val = max(val, minValue);
		val = min(val, maxValue);

		sv.clear();
		sv.str(L"");
		ZeroMemory(wText, sizeof(wchar_t)*256/*length*/);

		sv << val;
		sv >> wText;

		SetWindowText(hEdit, wText);
	}


	if (val == -INT_MAX) {
		//if (wText != nullptr)
		//	delete wText;
			
		return false;
	}

	//if (wText != nullptr)
	//	delete wText;

	return true;
}


NumericEdit *GetNumericEditCtrl(HWND hWnd)
{
	return static_cast<NumericEdit*>(reinterpret_cast<void*>(GetWindowLong(hWnd, 0)));
}

void SetNumericEditCtrl(HWND hWnd, NumericEdit *numEdit)
{
	SetWindowLong(hWnd, 0, reinterpret_cast<LONG>(numEdit));// Заменить на static_cast<LONG>(reinterpret_cast<void*>())
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
LRESULT NumericEdit::NumEditProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;

	//	wchar_t numSymbols[] = { L"0123456789.+-" };
	wchar_t text[256], temp[256];

	NumericEdit *nmEdit = GetNumericEditCtrl(hWnd);

	switch (message)
	{
	case WM_NCCREATE:
	{
		nmEdit = new NumericEdit();
		SetNumericEditCtrl(hWnd, nmEdit);
		//if((BOOL)lParam == TRUE) CreateWindow("UpDown")

		return (nmEdit != nullptr);
	}
	case WM_NCDESTROY:
		if (nmEdit != nullptr)
			delete nmEdit;
		break;
	case WM_KEYUP:
	{
		int minVal = 0, maxVal = 0, val = 0;
		std::wstringstream sv;

		if (nmEdit != nullptr)
		{
/*			if(nmEdit->hUpDown != nullptr)
				SendMessage(nmEdit->hUpDown,
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
				sv << val;
				sv >> text;

				SetWindowText(hWnd, text);

				return 0;
			}

			if(!((wParam >= '0' && wParam <= '9')
				|| wParam == '.'
				|| wParam == VK_RETURN
				|| wParam == VK_DELETE
				|| wParam == VK_BACK)){
				return 0;
			}*/
		}
		return 1; // обработать стандартным обработчиком класса Edit
	}
	default:
		break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}
