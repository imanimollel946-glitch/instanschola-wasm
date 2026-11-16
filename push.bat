@echo off
title InstanSchola - One-Click Git Push
color 0A

:: 1. Set the exact path to your InstanSchola project folder
set "PROJECT_DIR=C:\Users\YourName\source\repos\InstanSchola"

cd /d "%PROJECT_DIR%" || (
    color 0C
    echo [ERROR] Could not find project directory: %PROJECT_DIR%
    pause
    exit /b 1
)

echo ========================================================
echo   INSTANSCHOLA - AUTO GIT SYNC
echo ========================================================
echo.

:: 2. Check if there are actually any changes to push
git status --short
for /f %%i in ('git status --porcelain') do set HAS_CHANGES=1

if not defined HAS_CHANGES (
    echo [INFO] Working tree is clean. Nothing new to push!
    timeout /t 3 >nul
    exit /b 0
)

:: 3. Generate a clean timestamp for the commit message
for /f "tokens=2 delims==" %%I in ('wmic os get localdatetime /value') do set datetime=%%I
set "TIMESTAMP=%datetime:~0,4%-%datetime:~4,2%-%datetime:~6,2% %datetime:~8,2%:%datetime:~10,2%"

echo.
echo [1/3] Staging all modified and new files...
git add -A

echo [2/3] Committing changes...
git commit -m "Auto-update: %TIMESTAMP%"

echo [3/3] Pushing to remote repository...
git push

if %errorlevel% neq 0 (
    color 0C
    echo.
    echo [FAILED] Git push encountered an error! Check your connection or branch status.
    pause
    exit /b 1
)

echo.
echo [SUCCESS] InstanSchola synced to GitHub!
timeout /t 3 >nul