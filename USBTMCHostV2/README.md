Special thanks to NaoNaoMe for USBTMC drivers: https://github.com/NaoNaoMe/USBTMC-Host-Driver

Set min power, max power and circuit resistance in USBTMCHostV2.ino file.

Compile USBTMCHostV2.ino and upload to Arduino DUE (programming port) board

Board automatically reads voltage from A9; interpretes it as power, computes amperage and sends to Keysight power supply. A9 has 0-3.3V range
