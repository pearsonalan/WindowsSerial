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

constexpr int kMaxLineLength = 255;

TextWindow::TextWindow(winfx::Window* parent_window) :
	winfx::Window(winfx::loadString(IDC_TEXT_WINDOW), parent_window) {
	lines_.push_back(std::wstring());
}

void TextWindow::modifyWndClass(WNDCLASSEXW& wc) {
	wc.hCursor = ::LoadCursor(NULL, IDC_IBEAM);
	wc.hbrBackground = (HBRUSH)(COLOR_BTNSHADOW + 1);
}

LRESULT TextWindow::onCreate(HWND hwndParam, LPCREATESTRUCT lpCreateStruct) {
	winfx::Rect rect = getClientRect();
	setBufferLines(getVisibleLineCount(rect.height()));
	return winfx::Window::onCreate(hwndParam, lpCreateStruct);
}
	
LRESULT TextWindow::handleWindowMessage(HWND hwndParam, UINT uMsg, 
										WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		HANDLE_MSG(hwndParam, WM_PAINT, onPaint);
		HANDLE_MSG(hwndParam, WM_SIZE, onSize);
	}
	return Window::handleWindowMessage(hwndParam, uMsg, wParam, lParam);
}

// Create the font used to draw the text
HFONT TextWindow::createFont() {
	return CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, FIXED_PITCH, TEXT("Consolas"));
}

constexpr int kBorderHeight = 0;
constexpr int kBorderWidth = 2;

void TextWindow::onPaint(HWND hwnd) {
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwnd, &ps);

	HFONT font = createFont();

	// Draw the content of the text window
	SelectObject(hdc, font);
	SetTextColor(hdc, RGB(0, 0, 0));
	SetBkMode(hdc, TRANSPARENT); 

	winfx::Rect rect = getClientRect();

	int line_number = 0;
	const int line_height = getLineHeight();
	for (const std::wstring& line : lines_) {
		DrawTextW(hdc, line.c_str(), -1, 
			(LPRECT)winfx::Rect(kBorderWidth,
							    rect.height() + (line_number - buffer_lines_) * line_height,
								rect.width() - 2 * kBorderWidth,
								line_height),
			DT_NOCLIP);
		line_number++;
	}

	DeleteObject(font);
	EndPaint(hwnd, &ps);
}

LRESULT TextWindow::onSize(HWND hwnd, UINT state, int cx, int cy) {
	winfx::DebugOut(L"TextWindow: onSize state=%d, cx=%d, cy=%d\n", state, cx, cy);
	int line_count = getVisibleLineCount(cy);
	winfx::DebugOut(L"TextWindow: Line size = %d, line count = %d\n", line_height_, line_count);
	setBufferLines(line_count);
	return 0;
}

int TextWindow::getLineHeight() {
#if 0
	// If font size could be changed, compute height based on font
	if (line_height_ == 0) {
		HDC hdc = getDC();
		HFONT font = createFont();
		SelectObject(hdc, font);
		SIZE size;
		GetTextExtentPoint32W(hdc, L"M", 1, &size);
		releaseDC(hdc);
		line_height_ = size.cy;
	}
#endif
	return line_height_;
}

int TextWindow::getVisibleLineCount(int window_height) {
	return (window_height - 2 * kBorderHeight + getLineHeight() - 1) / getLineHeight();
}

void TextWindow::setBufferLines(int lines) {
	winfx::DebugOut(L"TextWindow: setting line count to %d\n", lines);
	buffer_lines_ = lines;
	while (lines_.size() > buffer_lines_) {
		lines_.pop_front();
	}
}

void TextWindow::appendData(const wchar_t* data, int len) {
	winfx::DebugOut(L"TextWindow appending %d chars\n", len);

	wchar_t line[kMaxLineLength + 1];

	// append data onto line until a newline char is reached
	int c = 0;
	for (const wchar_t* p = data; p - data < len; p++) {
		if (*p == L'\n') {
			appendTextToCurrentLine(std::wstring(line, c));
			advanceLine();
			c = 0;
		} else if (*p == L'\r') {
			// Eat \r characters... ignore CR-LF line endings
		} else {
			if (c < kMaxLineLength) {
				if (isprint(*p)) {
					line[c] = *p;
				} else {
					// Use a box character to represent an unprintable
					// character.
					line[c] = L'\u2B1C';
				}
				c++;
			} else {
				// eat the character
			}
		}
	}

	if (c > 0) {
		appendTextToCurrentLine(std::wstring(line, c));
	}
}

void TextWindow::appendTextToCurrentLine(const std::wstring& text) {
	std::wstring& current_line = lines_.back();
	current_line.append(text);
	::InvalidateRect(hwnd, NULL, TRUE);
}

void TextWindow::advanceLine() {
	lines_.push_back(std::wstring());
	while (lines_.size() > buffer_lines_) {
		lines_.pop_front();
	}
	::InvalidateRect(hwnd, NULL, TRUE);
}
