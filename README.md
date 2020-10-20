# WindowsSerial

Win32 test program based on code from https://aticleworld.com/serial-port-programming-using-win32-api/ 
which connects to the Arduino over the Serial Port and prints data received from the Arduiono.

This code uses a Baud rate of 115,200, which can be change by altering the parameters to SetCommState().

## Compiling

```
CMD> nmake
```

## Usage

Example:
```
serial.exe -port COM4
```