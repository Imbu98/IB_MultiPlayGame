@echo off

REM 언리얼 5.4 에디터 경로 (큰따옴표 없이 변수 지정)
SET ENGINE_PATH=D:\UE_5.5.4_FS\Engine\Binaries\Win64
REM 프로젝트 경로
SET PROJECT_PATH=D:\IB_MultiplayGame\IB_MultiPlayGame.uproject
REM 실행시킬 맵
SET MAP_NAME=/Game/Maps/ThirdPersonMap

REM 에디터에서 Dedicated Server 모드 실행
"%ENGINE_PATH%\UnrealEditor-Cmd.exe" "%PROJECT_PATH%" "%MAP_NAME%" -server -log -nosteam
pause