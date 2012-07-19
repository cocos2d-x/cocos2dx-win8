::Please Modify the variables into a suitable
::More Info visit: http:://www.cocos2d-x.org
@echo on

set PROJECT_NAME=helloworld-cocos2d
set COCOS2D_DIR=d:\cocos2dx-win8
set PROJECT_DIR=d:\Projects\helloworld-cocos2d

xcopy /S  "%COCOS2D_DIR%\tinyxml" "%PROJECT_DIR%\tinyxml\"
xcopy /S  "%COCOS2D_DIR%\Box2D" "%PROJECT_DIR%\Box2D\"
xcopy /S  "%COCOS2D_DIR%\chipmunk" "%PROJECT_DIR%\chipmunk\"
xcopy /S  "%COCOS2D_DIR%\cocos2dx" "%PROJECT_DIR%\cocos2dx\"
xcopy /S  "%COCOS2D_DIR%\CocosDenshion" "%PROJECT_DIR%\CocosDenshion\"
xcopy /S  "%COCOS2D_DIR%\.gitignore" "%PROJECT_DIR%\"
xcopy /S  "%COCOS2D_DIR%\HelloWorld\Classes" "%PROJECT_DIR%\%PROJECT_NAME%\Classes\"
xcopy /S  "%COCOS2D_DIR%\HelloWorld\AppDelegate.cpp" "%PROJECT_DIR%\%PROJECT_NAME%"
xcopy /s  "%COCOS2D_DIR%\HelloWorld\AppDelegate.h" "%PROJECT_DIR%\%PROJECT_NAME%"
