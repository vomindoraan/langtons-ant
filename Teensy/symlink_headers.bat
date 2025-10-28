@echo off
rem Workaround for Arduino IDE's lackluster project file system

cd "%~dp0"

set src_dir=..\LangtonsAnt
set dst_dirs=.
set files=serial.h io.h logic.h

for %%d in (%dst_dirs%) do (
    for %%f in (%files%) do (
        del /q %%d\%%f
        mklink %%d\%%f %src_dir%\%%f
    )
)
