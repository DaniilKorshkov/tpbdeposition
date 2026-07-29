Special thanks to NaoNaoMe for USBTMC drivers: https://github.com/NaoNaoMe/USBTMC-Host-Driver

Set min power, max power and circuit resistance in USBTMCHostV2.ino file.

Compile USBTMCHostV2.ino and upload to Arduino DUE (programming port) board

Board automatically reads voltage from A9; interpretes it as power, computes amperage and sends to Keysight power supply(s). A9 has 0-3.3V range

Multiple USBTMC slaves are supported via a USB hub. Up to 8 devices (`MAX_USBTMC_DEVICES`). Each connected slave receives the same CURR command every loop cycle.
