rem c:\Python27\Scripts\mbed.exe config -G MBED_OS_DIR mbed-os-5.11.5
rem c:\Python27\Scripts\mbed.exe new labFlowchannel --create-only
rem c:\Python27\Scripts\mbed.exe export -m AUTO -i uvision5 --source labFlowchannel --source mbed-os-5.11.5 --source libSDP3x

cd .\labFlowChannel
..\mbed-os_v152spez\tools\project.py -m NUCLEO_L432KC -i uvision5 --source . --source ..\libSDP3x --source ..\mbed-os_v152spez
cd ..

pause
