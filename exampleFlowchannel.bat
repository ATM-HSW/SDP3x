mbed config -G MBED_OS_DIR mbed-os-5.12.0
mbed new exampleFlowchannel --create-only
mbed export -m NUCLEO_F446RE -i uvision6 --source exampleFlowchannel --source mbed-os-5.12.0 --source libSDP3x

pause
