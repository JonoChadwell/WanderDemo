cd %~dp0

SET PATH=%PATH%;C:\emsdk;C:\Program Files (x86)\GnuWin32\bin;

call emsdk_env.bat

make.exe wander

pause