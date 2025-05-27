@echo off

REM 언리얼 5.4 에디터 경로 (큰따옴표 없이 변수 지정)
SET ENGINE_PATH=D:\UE_5.5.4_FS\Engine\Binaries\Win64
SET PROJECT_PATH=D:\IB_MultiplayGame\IB_MultiPlayGame.uproject
SET MAP_NAME=/Game/Maps/ThirdPersonMap

REM 에디터에서 Game 실행 모드 실행
"%ENGINE_PATH%\UnrealEditor-Cmd.exe" "%PROJECT_PATH%" "%MAP_NAME%" -game -log -nosteam
pause