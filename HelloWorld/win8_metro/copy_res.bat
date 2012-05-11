@echo on
set SRC_DIR=..\Resource
set DST_DIR=%1

xcopy /S /Y %SRC_DIR% %DST_DIR%
