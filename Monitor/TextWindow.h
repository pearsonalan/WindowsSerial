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

class TextWindow : public winfx::Window {
public:
	explicit TextWindow(winfx::Window* parent_window); 

	void modifyWndClass(WNDCLASSEXW& wc) override;

	LRESULT handleWindowMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;

	LRESULT onCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct) override;
	// Add some data to the output
	void appendData(const wchar_t* data, int len);

protected:
	// Window message handlers
	void onPaint(HWND hwnd);
	LRESULT onSize(HWND hwnd, UINT state, int cx, int cy);

	// Creates the font used to draw the text
	HFONT createFont();

	// Appends the given text to the current line
	void appendTextToCurrentLine(const std::wstring& text);

	// Ends the current line and advance to a new line
	void advanceLine();

	// Compute the height of a line of text
	int getLineHeight();

	// Compute the number of visible (or partially visible) lines
	int getVisibleLineCount(int window_height);

	// Set the number of lines to keep in the buffer
	void setBufferLines(int lines);

	// Memoized line height. A value of 0 means the height has not been computed yet
	// For now, we don't support changing font size so this is a constant.
	int line_height_ = 17;

	// Number of lines to buffer
	int buffer_lines_ = 24;

	std::deque<std::wstring> lines_;
};
