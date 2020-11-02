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

#include "winfx.h"

constexpr int kReadBufferSize = 256;

class Serial {
public:
	Serial(int baud_rate = 9600) : baud_rate_(baud_rate) {
		::ZeroMemory(&overlapped_, sizeof(OVERLAPPED));
	}
	Serial(const std::wstring& port_file_name, int baud_rate = 9600) :
		port_file_name_(port_file_name), baud_rate_(baud_rate) {
		::ZeroMemory(&overlapped_, sizeof(OVERLAPPED));
	}

	virtual ~Serial() { close(); }

	void setComPort(const std::wstring& port_file_name) {
		port_file_name_ = port_file_name;
	}

	void setBaudRate(int baud_rate) {
		baud_rate_ = baud_rate;
	}

	// Open the serial com port
	HRESULT open();

	// Starts an asyncronous (overlapped) read operation on the serial
	// port.
	HRESULT startAsyncRead();

	// Invoked when an async read completes successfully
	HRESULT onAsyncReadCompleted();

	void close();

private:
	// Initialize the com port after opening.
	HRESULT initializePort();

	HRESULT setCommTimeouts();

	// Process the data buffer from a read operation. The data is in
	// the read_buffer_ instance variable. The byte_count paramter
	// indicates how much data is available in read_buffer_.
	void processReadBuffer(DWORD byte_count);

	// Log an error message to DebugOut and return an HRESULT for
	// the error code returned by GetLastError()
	HRESULT error(const std::wstring& message);

	// Communications settings
	std::wstring port_file_name_;
	int baud_rate_ = 0;

	HANDLE com_port_ = INVALID_HANDLE_VALUE;
	OVERLAPPED overlapped_;
	HANDLE event_ = INVALID_HANDLE_VALUE;

	BYTE read_buffer_[kReadBufferSize] = { 0 };
};