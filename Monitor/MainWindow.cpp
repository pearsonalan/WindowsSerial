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

#include "MainWindow.h"
#include "PreferencesDialog.h"
#include "Registry.h"
#include "Monitor.h"

void MainWindow::modifyWndClass(WNDCLASSEXW& wc) {
	wc.lpszMenuName = MAKEINTRESOURCE(IDC_MONITOR_APP);
}

winfx::Size MainWindow::getDefaultWindowSize() {
	RegistryKey settings(RegistryKey::CurrentUser, kRegistryKeyName);
	if (settings != RegistryKey::InvalidKey) {
		return winfx::Size(settings.getIntegerValueOrDefault(L"WindowCX", CW_USEDEFAULT),
						   settings.getIntegerValueOrDefault(L"WindowCY", CW_USEDEFAULT));
	}
	return winfx::Size(CW_USEDEFAULT, CW_USEDEFAULT);
}

LRESULT MainWindow::handleWindowMessage(HWND hwndParam, UINT uMsg, 
										WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		HANDLE_MSG(hwndParam, WM_COMMAND, onCommand);
		HANDLE_MSG(hwndParam, WM_DESTROY, onDestroy);
		HANDLE_MSG(hwndParam, WM_PAINT, onPaint);
		HANDLE_MSG(hwndParam, WM_SIZE, onSize);
	}
	return Window::handleWindowMessage(hwndParam, uMsg, wParam, lParam);
}

LRESULT MainWindow::onCreate(HWND hwndParam, LPCREATESTRUCT lpCreateStruct) {
	connectSerial();
	status_window_.createChildWindow(winfx::Point(0, 20), winfx::Size(100, 100), IDC_STATUS_WINDOW);
	text_window_.createChildWindow(winfx::Point(0, 0), winfx::Size(20, 20), IDC_TEXT_WINDOW);
	return winfx::Window::onCreate(hwndParam, lpCreateStruct);
}

void MainWindow::onCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify) {
	switch (id) {
	case IDM_PREFERENCES:
		PreferencesDialog(this).doDialogBox();
		break;

	case IDM_ABOUT:
		AboutDialog(this).doDialogBox();
		break;

	case IDM_EXIT:
		destroy();
		break;
	}
}

void MainWindow::onPaint(HWND hwnd) {
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwnd, &ps);

	EndPaint(hwnd, &ps);
}

LRESULT MainWindow::onSize(HWND hwnd, UINT state, int cx, int cy) {
	winfx::DebugOut(L"onSize state=%d, cx=%d, cy=%d", state, cx, cy);
	if (state == SIZE_RESTORED) {
		status_window_.moveWindow(winfx::Point(0, cy-20), winfx::Size(cx, 20), true);
		text_window_.moveWindow(winfx::Point(0, 0), winfx::Size(cx, cy-20), true);
	}
	return 0;
}

LRESULT MainWindow::onClose(HWND hwnd) {
	winfx::DebugOut(L"onClose");
	saveWindowSize();
	PostQuitMessage(0);
	return 0;
}

void MainWindow::onDestroy(HWND hwnd) {
	winfx::DebugOut(L"onDestroy");
	saveWindowSize();
	PostQuitMessage(0);
}

void MainWindow::connectSerial() {
	serial_.setComPort(L"\\\\.\\COM4");
	serial_.setBaudRate(115200);

	HRESULT hr;
	if (FAILED(hr = serial_.open())) {
		winfx::DebugOut(L"Could not open serial port. Error %08x", hr);
	}
}

void MainWindow::saveWindowSize() {
	WINDOWPLACEMENT placement;
	placement.length = sizeof(WINDOWPLACEMENT);
	::GetWindowPlacement(hwnd, &placement);

	winfx::DebugOut(L"placement: x=%d, y=%d, cx=%d, cy=%d, show=%d",
		placement.rcNormalPosition.left,
		placement.rcNormalPosition.top,
		placement.rcNormalPosition.right - placement.rcNormalPosition.left,
		placement.rcNormalPosition.bottom - placement.rcNormalPosition.top,
		placement.showCmd);

	// Save window size so it can be reopened to the same size
	winfx::Rect r = getWindowRect();
	RegistryKey settings = RegistryKey::CurrentUser.openOrCreate(kRegistryKeyName);
	if (settings != RegistryKey::InvalidKey) {
		settings.setIntegerValue(L"WindowCX", r.width());
		settings.setIntegerValue(L"WindowCY", r.height());
	}
}
