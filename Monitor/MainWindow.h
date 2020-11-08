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

#pragma once

#include "framework.h"
#include "winfx.h"
#include "Resource.h"
#include "Serial.h"
#include "StatusWindow.h"
#include "TextWindow.h"

class MainWindow : public winfx::Window, public SerialNotificationSink {
public:
	MainWindow() : 
		winfx::Window(winfx::loadString(IDS_APP_CLASS), winfx::loadString(IDS_APP_TITLE)),
		status_window_(this),
		text_window_(this) {}
	virtual ~MainWindow() {}

	void modifyWndClass(WNDCLASSEXW& wc) override;
	winfx::Size getDefaultWindowSize() override;

	LRESULT handleWindowMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;

	LRESULT onCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct) override;
	LRESULT onClose(HWND hwnd) override;

	void onReceivedData(const wchar_t* data, int len) override;
	
protected:
	void connectSerial();

	// Save window size so it can be reopened to the same size
	void saveWindowSize();

	void onCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
	void onDestroy(HWND hwnd);
	void onPaint(HWND hwnd);
	LRESULT onSize(HWND hwnd, UINT state, int cx, int cy);

	Serial serial_;
	StatusWindow status_window_;
	TextWindow text_window_;
};

class AboutDialog : public winfx::Dialog {
public:
	AboutDialog(Window* pwnd) : winfx::Dialog(pwnd, IDD_ABOUTBOX) {}
};
