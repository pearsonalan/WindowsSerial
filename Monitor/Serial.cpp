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
HRESULT Serial::open() {
	if (com_port_ != INVALID_HANDLE_VALUE) {
		close();
	}

	if (port_file_name_.empty()) {
		winfx::DebugOut(L"Have not set the comm port");
		return E_INVALIDARG;
	}

	com_port_ = ::CreateFile(port_file_name_.c_str(),
							 GENERIC_READ | GENERIC_WRITE, 0,
							 NULL, OPEN_EXISTING,
							 FILE_FLAG_OVERLAPPED, NULL);
	if (com_port_ == INVALID_HANDLE_VALUE) {
		return error(L"Failed to open port");
	}

	HRESULT hr = initializePort();
	if (FAILED(hr)) return hr;

	hr = setCommTimeouts();
	if (FAILED(hr)) return hr;

	winfx::DebugOut(L"Opened com port %s", port_file_name_.c_str());

	event_ = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	overlapped_.hEvent = event_;

	winfx::App::getSingleton().addEventHandler(event_, 
		std::bind(&Serial::onAsyncReadCompleted, this));

	return startAsyncRead();
}

HRESULT Serial::initializePort() {
	DCB dcbSerialParams = { 0 };
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	if (!::GetCommState(com_port_, &dcbSerialParams)) {
		return error(L"GetCommState failed");
	}

	dcbSerialParams.BaudRate = baud_rate_;
	dcbSerialParams.ByteSize = 8;
	dcbSerialParams.StopBits = ONESTOPBIT;
	dcbSerialParams.Parity = NOPARITY;
	if (!::SetCommState(com_port_, &dcbSerialParams)) {
		return error(L"SetCommState failed");
	}

	return S_OK;
}

HRESULT Serial::setCommTimeouts() {
	// Set Timeouts
	COMMTIMEOUTS timeouts = { 0 };
	timeouts.ReadIntervalTimeout = 5;
	timeouts.ReadTotalTimeoutConstant = 5;
	timeouts.ReadTotalTimeoutMultiplier = 100;
	timeouts.WriteTotalTimeoutConstant = 50;
	timeouts.WriteTotalTimeoutMultiplier = 10;
	if (!::SetCommTimeouts(com_port_, &timeouts)) {
		return error(L"SetCommTimeouts failed");
	}
	return S_OK;
}

HRESULT Serial::startAsyncRead() {
	if (com_port_ == INVALID_HANDLE_VALUE) {
		winfx::DebugOut(L"Cannot start Serial read. Com port is not open");
		return E_INVALIDARG;
	}

	// Start an overlapped read operation. If the read succeeds immediately, we process
	// the buffer and try to start an async read again.
	for (;;) {
		DWORD bytes_transferred = 0;
		if (::ReadFile(com_port_, &read_buffer_, kReadBufferSize,
					   &bytes_transferred, &overlapped_)) {
			// Read was successful. Process the data.
			processReadBuffer(bytes_transferred);
		} else {
			DWORD error_code = GetLastError();
			if (error_code == ERROR_IO_PENDING) {
				// The async operation is pending.
				return S_OK;
			} else {
				// A different failure happened.
				winfx::DebugOut(L"Serial[%s]: Error %08X in ReadFile",
								port_file_name_.c_str(), error_code);
				return HRESULT_FROM_WIN32(error_code);
			}
		}
	}
}

HRESULT Serial::onAsyncReadCompleted() {
	// Find out the result of the read.
	DWORD bytes_transferred = 0;
	if (::GetOverlappedResult(com_port_, &overlapped_, &bytes_transferred, FALSE)) {
		if (bytes_transferred > 0) {
			processReadBuffer(bytes_transferred);
		}
	}
	startAsyncRead();
	return S_OK;
}

void Serial::processReadBuffer(DWORD byte_count) {
	winfx::DebugOut(L"Serial[%s]: read %d bytes", port_file_name_.c_str(), byte_count);

	// Convert the bytes to Unicode wide chars
	wchar_t buffer[kReadBufferSize + 1];
	int chars_converted = MultiByteToWideChar(437, MB_PRECOMPOSED,
		(LPCCH)read_buffer_, byte_count,
		buffer, kReadBufferSize + 1);
	if (chars_converted > 0) {
		// Null terminate string
		buffer[chars_converted] = L'\0';
		if (notification_sink_) {
			notification_sink_->onReceivedData(buffer, chars_converted);
		}
	}
}

HRESULT Serial::error(const std::wstring& message) {
	DWORD error = ::GetLastError();
	winfx::DebugOut(L"Serial[%s]: %s: Error %08X",
					port_file_name_.c_str(), message.c_str(), error);
	return HRESULT_FROM_WIN32(error);
}

void Serial::close() {
	if (com_port_ != INVALID_HANDLE_VALUE) {
		CloseHandle(com_port_);
		com_port_ = INVALID_HANDLE_VALUE;
	}

	if (event_ != INVALID_HANDLE_VALUE) {
		CloseHandle(event_);
		event_ = INVALID_HANDLE_VALUE;
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

#endif
