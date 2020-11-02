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

class Serial {
public:
	Serial(int baud_rate = 9600) : baud_rate_(baud_rate) {}
	Serial(const std::wstring& port_file_name, int baud_rate = 9600) :
		port_file_name_(port_file_name), baud_rate_(baud_rate) {}

	virtual ~Serial() { close(); }

	void setComPort(const std::wstring& port_file_name) {
		port_file_name_ = port_file_name;
	}

	void setBaudRate(int baud_rate) {
		baud_rate_ = baud_rate;
	}
	
	// Open the serial com port
	HRESULT open();

	void close();

private:
	// Initialize the com port after opening.
	HRESULT initializePort();

	// Log an error message to DebugOut and return an HRESULT for
	// the error code returned by GetLastError()
	HRESULT error(const std::wstring& message);

	std::wstring port_file_name_;
	HANDLE com_port_ = INVALID_HANDLE_VALUE;
	int baud_rate_ = 0;
};