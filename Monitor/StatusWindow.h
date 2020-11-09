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

#pragma once

#include "winfx.h"
#include "Resource.h"

class StatusWindow : public winfx::Window {
public:
	explicit StatusWindow(winfx::Window* parent_window) : 
		winfx::Window(winfx::loadString(IDC_STATUS_WINDOW), parent_window) {
	}

	void modifyWndClass(WNDCLASSEXW& wc) override;
	LRESULT handleWindowMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;

	void setStatusMessage(const std::wstring& status_message);
	void setComPort(const std::wstring& com_port);
	void setBaudRate(int baud_rate);

protected:
	void onPaint(HWND hwnd);

	std::wstring status_message_;
	std::wstring com_port_;
	std::wstring baud_rate_;
};
