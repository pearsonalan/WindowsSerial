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

constexpr int kSettingsRectWidth = 200;
constexpr int kSettingsRectHeight = 16;
constexpr int kSettingsRectOffset = 2;

void StatusWindow::onPaint(HWND hwnd) {
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwnd, &ps);
	winfx::Rect client_rect = getClientRect();

	HFONT status_font = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, VARIABLE_PITCH, TEXT("Arial"));

	HFONT settings_font = CreateFont(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, VARIABLE_PITCH, TEXT("Arial"));

	// Draw the status message
	HGDIOBJ original_font = SelectObject(hdc, status_font);
	SetTextColor(hdc, RGB(0, 0, 0));
	SetBkMode(hdc, TRANSPARENT); 
	DrawTextW(hdc, status_message_.c_str(), -1, (LPRECT)winfx::Rect(1, 1, 30, 18),
		DT_NOCLIP);

	// Draw the settings box
	winfx::Rect settings_rect(client_rect.width() - kSettingsRectWidth - kSettingsRectOffset, 
							  kSettingsRectOffset, 
							  client_rect.width() - kSettingsRectOffset, 
							  kSettingsRectOffset + kSettingsRectHeight); 
	DrawEdge(hdc, (LPRECT)settings_rect, EDGE_SUNKEN, BF_RECT); 
	SelectObject(hdc, settings_font);
	
	// Draw com port setting
	// Adjust settings rect to be position of com port
	settings_rect.left += 6;
	settings_rect.top += 1;
	settings_rect.bottom -= 1;
	settings_rect.right = settings_rect.left + 80;
	DrawTextW(hdc, com_port_.c_str(), -1, (LPRECT)settings_rect, DT_NOCLIP);

	// Draw baud rate settings
	// Adjust settings rect to be position of baud_rate
	settings_rect.left = settings_rect.right + 20;
	settings_rect.right = settings_rect.left + 80;
	DrawTextW(hdc, baud_rate_.c_str(), -1, (LPRECT)settings_rect, DT_NOCLIP);

	SelectObject(hdc, original_font);

	DeleteObject(status_font);
	DeleteObject(settings_font);
	EndPaint(hwnd, &ps);
}

void StatusWindow::setStatusMessage(const std::wstring& status_message) {
	status_message_ = status_message;
	invalidateRect();
}

void StatusWindow::setComPort(const std::wstring& com_port) {
	com_port_ = com_port;
	invalidateRect();
}

void StatusWindow::setBaudRate(int baud_rate) {
	wchar_t buffer[16];
	_itow_s(baud_rate, buffer, 10);
	baud_rate_ = buffer;
	invalidateRect();
}