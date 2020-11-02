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

#include "StatusWindow.h"

void StatusWindow::modifyWndClass(WNDCLASSEXW& wc) {
	wc.hCursor = ::LoadCursor(NULL, IDC_HAND);
	wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
}

LRESULT StatusWindow::handleWindowMessage(HWND hwndParam, UINT uMsg, 
										  WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		HANDLE_MSG(hwndParam, WM_PAINT, onPaint);
	}
	return Window::handleWindowMessage(hwndParam, uMsg, wParam, lParam);
}

void StatusWindow::onPaint(HWND hwnd) {
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwnd, &ps);
	winfx::Rect client_rect = getClientRect();

	HFONT hFont = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, VARIABLE_PITCH, TEXT("Arial"));

	// Draw the status message
	SelectObject(hdc, hFont);
	SetTextColor(hdc, RGB(0, 0, 0));
	SetBkMode(hdc, TRANSPARENT); 

	DrawTextW(hdc, L"Status message", -1, (LPRECT)winfx::Rect(1, 1, 30, 18),
		DT_NOCLIP);

	DrawEdge(hdc, 
		(LPRECT)winfx::Rect(client_rect.width() - 200 - 4, 2, 
							client_rect.width() - 2, 18), 
		EDGE_SUNKEN, BF_RECT);

	DeleteObject(hFont);
	EndPaint(hwnd, &ps);
}