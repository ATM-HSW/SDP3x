rem c:\Python27\Scripts\mbed config -G MBED_OS_DIR mbed-os-5.11.5
rem c:\Python27\Scripts\mbed new exampleFlowchannel --create-only
rem c:\Python27\Scripts\mbed export -m AUTO -i uvision5 --source exampleFlowchannel --source mbed-os-5.11.5 --source libSDP3x

..\mbed-os_v152\tools\project.py -m NUCLEO_L432KC -i uvision5 --source . --source ..\libSDP3x --source ..\mbed-os_v152

pause
