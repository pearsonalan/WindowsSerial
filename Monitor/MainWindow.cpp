// Copyright(C) 2020 Alan Pearson
//
// This program is free software : you can redistribute it and /or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
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
		if (PreferencesDialog(this).doDialogBox() == IDOK) {
			// Preferences may have changed, re-connect to serial port
			connectSerial();
		}
		break;

	case IDM_ABOUT:
		AboutDialog(this).doDialogBox();
		break;

	case IDM_EXIT:
		destroy();
		break;

	case IDM_CONNECT:
		connectSerial();
		break;

	case IDM_DISCONNECT:
		disconnectSerial();
		break;
	}
}

void MainWindow::onPaint(HWND hwnd) {
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwnd, &ps);

	EndPaint(hwnd, &ps);
}

LRESULT MainWindow::onSize(HWND hwnd, UINT state, int cx, int cy) {
	winfx::DebugOut(L"onSize state=%d, cx=%d, cy=%d\n", state, cx, cy);
	if (state == SIZE_RESTORED) {
		status_window_.moveWindow(winfx::Point(0, cy-20), winfx::Size(cx, 20), true);
		text_window_.moveWindow(winfx::Point(0, 0), winfx::Size(cx, cy-20), true);
	}
	return 0;
}

LRESULT MainWindow::onClose(HWND hwnd) {
	winfx::DebugOut(L"onClose\n");
	saveWindowSize();
	PostQuitMessage(0);
	return 0;
}

void MainWindow::onDestroy(HWND hwnd) {
	winfx::DebugOut(L"onDestroy\n");
	saveWindowSize();
	PostQuitMessage(0);
}

void MainWindow::connectSerial() {
	std::wstring com_port = kDefaultComPort;
	int baud_rate = kDefaultBaudRate;

	RegistryKey settings_key = RegistryKey::CurrentUser.openOrCreate(kRegistryKeyName);
	if (settings_key != RegistryKey::InvalidKey) {
		com_port = settings_key.getStringValueOrDefault(kPortRegistryValueName, kDefaultComPort);
		baud_rate = settings_key.getIntegerValueOrDefault(kBaudRateRegistryValueName, kDefaultBaudRate);
	}

	status_window_.setComPort(com_port);
	status_window_.setBaudRate(baud_rate);

	std::wstring com_port_filename = std::wstring(L"\\\\.\\") + com_port;
	serial_.setComPort(com_port_filename);
	serial_.setBaudRate(baud_rate);
	serial_.setNotificationSink(this);

	HRESULT hr;
	if (FAILED(hr = serial_.open())) {
		wchar_t buffer[80];
		swprintf_s(buffer, L"Could not open serial port. Error %08x", hr);
		status_window_.setStatusMessage(buffer);
		winfx::DebugOut(L"%s\n", buffer);
	} else {
		status_window_.setStatusMessage(L"Connected");
	}
}

void MainWindow::disconnectSerial() {
	// Disconnect triggered by user.
	serial_.close();
	status_window_.setStatusMessage(L"Disconnected");
}

void MainWindow::onReceivedData(const wchar_t* data, int len) {
	// winfx::DebugOut(L"SER->MW: READ: %s\n", data);
	text_window_.appendData(data, len);
}

void MainWindow::onDisconnected() {
	// Disconnect notificaction from I/O failure.
	winfx::DebugOut(L"MainWindow notified of disconnect");
	serial_.close();
	status_window_.setStatusMessage(L"Disconnected");
}	

void MainWindow::saveWindowSize() {
	WINDOWPLACEMENT placement;
	placement.length = sizeof(WINDOWPLACEMENT);
	::GetWindowPlacement(hwnd, &placement);

	winfx::DebugOut(L"placement: x=%d, y=%d, cx=%d, cy=%d, show=%d\n",
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
