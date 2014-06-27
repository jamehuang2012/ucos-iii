@echo off
color 2e
chdir /d %cd%

for %%i in (%chdir%*.*) do (
 if not "%%~xi"==".cmd" (
 if not "%%~xi"==".hex" (
 if not "%%~xi"==".uvopt" (
 if not "%%~xi"==".uvproj" (
 if not "%%~xi"==".s" (
 if not "%%~xi"==".c" (
 if not "%%~xi"==".h" (
 if not "%%~xi"==".txt" @del *%%~xi
)
)
)
)
)
)
)
)
exit