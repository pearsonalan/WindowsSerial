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

#include "winfx.h"
#include "Serial.h"

// Open the serial com port
HRESULT Serial::Open() {
	if (com_port_ != INVALID_HANDLE_VALUE) {
		Close();
	}

	com_port_ = ::CreateFile(port_file_name_.c_str(),
							 GENERIC_READ | GENERIC_WRITE, 0,
							 NULL, OPEN_EXISTING, 0, NULL);
	if (com_port_ == INVALID_HANDLE_VALUE) {
		return Error(L"Failed to open port");
	}

	HRESULT hr = SetBaudRate();
	if (FAILED(hr)) return hr;

	return S_OK;
}

HRESULT Serial::SetBaudRate() {
	DCB dcbSerialParams = { 0 };
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	if (!::GetCommState(com_port_, &dcbSerialParams)) {
		return Error(L"GetCommState failed");
	}

	dcbSerialParams.BaudRate = baud_rate_;
	dcbSerialParams.ByteSize = 8;
	dcbSerialParams.StopBits = ONESTOPBIT;
	dcbSerialParams.Parity = NOPARITY;
	if (!::SetCommState(com_port_, &dcbSerialParams)) {
		return Error(L"SetCommState failed");
	}

	return S_OK;
}

HRESULT Serial::Error(const std::wstring& message) {
	DWORD error = ::GetLastError();
	winfx::DebugOut(L"Serial[%s]: %s: Error %08X",
					port_file_name_.c_str(), message.c_str(), error);
	return HRESULT_FROM_WIN32(error);
}

void Serial::Close() {
	if (com_port_ != INVALID_HANDLE_VALUE) {
		CloseHandle(com_port_);
		com_port_ = INVALID_HANDLE_VALUE;
	}
}

#if 0
BOOL Serial::WriteMessage(HANDLE com_port, std::string message) {
	DWORD bytes_written;
	if (!WriteFile(com_port, message.c_str(), message.length(),
				   &bytes_written, NULL)) {
		return FALSE;
	}
	return TRUE;
}


Serial::SetCommTimeouts() {
	// Set Timeouts
	COMMTIMEOUTS timeouts = { 0 };
	timeouts.ReadIntervalTimeout = 50;
	timeouts.ReadTotalTimeoutConstant = 50;
	timeouts.ReadTotalTimeoutMultiplier = 10;
	timeouts.WriteTotalTimeoutConstant = 50;
	timeouts.WriteTotalTimeoutMultiplier = 10;
	if (!SetCommTimeouts(com_port, &timeouts)) {
		std::cout << "Error " << GetLastError() << " in SetCommTimeouts" << std::endl;
		CloseHandle(com_port);
		return -1;
	}
}

Serial::SetReceiveMask() {
	// Set Receive Mask
	if (!SetCommMask(com_port, EV_RXCHAR)) {
		std::cout << std::endl << "Error " << GetLastError() << " in SetCommMask" << std::endl;
		break;
	}
}

Serial::WaitForCommEvent() {
	// Wait for COMM event
	DWORD event_mask;
	if (!WaitCommEvent(com_port, &event_mask, NULL)) {
		std::cout << std::endl << "Error " << GetLastError() << " in WaitCommEvent" << std::endl;
		break;
	}
}


Serial::Read() {
	const int BUFFER_SIZE = 1024;
	CHAR buffer[BUFFER_SIZE + 1] = { 0 };
	DWORD bytes_read;
	if (!ReadFile(com_port, &buffer, BUFFER_SIZE, &bytes_read, NULL)) {
		std::cout << std::endl << "Error " << GetLastError() << " in ReadFile" << std::endl;
		break;
	}
	if (bytes_read > 0) {
		buffer[bytes_read] = 0;
		std::cout << buffer;
	} else {
		std::cout << std::endl << "Read " << bytes_read << " bytes" << std::endl;
		break;
	}
}

#endif
