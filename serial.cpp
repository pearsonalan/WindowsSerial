#include <Windows.h>

#include <string>
#include <iostream>

BOOL WriteMessage(HANDLE com_port, std::string message) {
    DWORD bytes_written;
    if (!WriteFile(com_port, message.c_str(), message.length(),
                   &bytes_written, NULL)) {
        return FALSE;
    }
    return TRUE;
}

void usage() {
    std::cout << "Usage:" << std::endl;
    std::cout << "    serial -port COM_N" << std::endl;
}

int main(int argc, char** argv) {
    HANDLE com_port;
    std::string port_file_name;
    std::string port_name;

    // Parse the command line args for the port
    for (--argc, ++argv; argc; --argc, ++argv) {
        std::string arg(*argv);
        if (arg[0] != '-') break;
        if (arg == "-port") {
            --argc, ++argv;
            if (!argc) {
                break;
            }
            port_name = std::string(*argv);
        }
    }

    if (port_name.empty()) {
        usage();
        return -1;
    }

    port_file_name = std::string("\\\\.\\") + port_name;

    std::cout << "Connecting to Arduino on port " << port_file_name << std::endl;

    // Open the serial com port
    com_port = CreateFile(port_file_name.c_str(), GENERIC_READ | GENERIC_WRITE, 0,
                          NULL, OPEN_EXISTING, 0, NULL);
    if (com_port == INVALID_HANDLE_VALUE) {
        std::cout << "Error " << GetLastError() << " opening port " << port_file_name << std::endl;
        return -1;
    }

    // Get Parameters for the SerialPort
    DCB dcbSerialParams = { 0 };
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(com_port, &dcbSerialParams)) {
        std::cout << "Error " << GetLastError() << " in GetCommState" << std::endl;
        CloseHandle(com_port);
        return -1;
    }

    // Set Parameters
    dcbSerialParams.BaudRate = CBR_115200;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    if (!SetCommState(com_port, &dcbSerialParams)) {
        std::cout << "Error " << GetLastError() << " in SetCommState" << std::endl;
        CloseHandle(com_port);
        return -1;
    }

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

    for (;;) {
        // Set Receive Mask
        if (!SetCommMask(com_port, EV_RXCHAR)) {
            std::cout << std::endl << "Error " << GetLastError() << " in SetCommMask" << std::endl;
            break;
        }

        // Wait for COMM event
        DWORD event_mask;
        if (!WaitCommEvent(com_port, &event_mask, NULL)) {
            std::cout << std::endl << "Error " << GetLastError() << " in WaitCommEvent" << std::endl;
            break;
        }

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

    CloseHandle(com_port);
    return 0;
}

