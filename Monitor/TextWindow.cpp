// Copyright(C) 2020 Alan Pearson
//
// This program is free software : you can redistribute it and /or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.If not, see < https://www.gnu.org/licenses/>.

#include "TextWindow.h"

void TextWindow::modifyWndClass(WNDCLASSEXW& wc) {
	wc.hCursor = ::LoadCursor(NULL, IDC_IBEAM);
	wc.hbrBackground = (HBRUSH)(COLOR_BTNSHADOW + 1);
}

LRESULT TextWindow::handleWindowMessage(HWND hwndParam, UINT uMsg, 
										WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		HANDLE_MSG(hwndParam, WM_PAINT, onPaint);
	}
	return Window::handleWindowMessage(hwndParam, uMsg, wParam, lParam);
}

void TextWindow::onPaint(HWND hwnd) {
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwnd, &ps);

	HFONT hFont = CreateFont(18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, FIXED_PITCH, TEXT("Consolas"));


	// Draw the status message
	SelectObject(hdc, hFont);
	SetTextColor(hdc, RGB(0, 0, 0));
	SetBkMode(hdc, TRANSPARENT); 

	DrawTextW(hdc, L"Hello World", -1, (LPRECT)winfx::Rect(10, 10, 300, 30),
		DT_NOCLIP);

	DeleteObject(hFont);
	EndPaint(hwnd, &ps);
}