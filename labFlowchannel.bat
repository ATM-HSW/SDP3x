mbed config -G MBED_OS_DIR mbed-os-5.12.4
mbed new labFlowchannel --create-only
mbed export -m NUCLEO_F446RE -i uvision6 --source labFlowchannel --source mbed-os-5.12.4 --source libSDP3x

pause
