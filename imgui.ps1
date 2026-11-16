$ErrorActionPreference = "Stop"
Write-Host "=== Study Learn: Direct Dependency Linker ===" -ForegroundColor Cyan

# Automatically locate vcpkg
$vcpkgDir = "C:\vcpkg"
if (!(Test-Path "$vcpkgDir\vcpkg.exe")) {
    $altPath = "$env:USERPROFILE\vcpkg"
    if (Test-Path "$altPath\vcpkg.exe") {
        $vcpkgDir = $altPath
    } else {
        throw "vcpkg.exe could not be found at C:\vcpkg or your user profile. Please ensure vcpkg is installed."
    }
}

Write-Host "Found vcpkg at: $vcpkgDir" -ForegroundColor Green

# 1. Integrate vcpkg with Visual Studio
Write-Host "[1/3] Integrating vcpkg with Visual Studio..." -ForegroundColor Cyan
& "$vcpkgDir\vcpkg.exe" integrate install

# 2. Install/Update Raylib and latest Dear ImGui
Write-Host "[2/3] Fetching latest raylib and imgui via vcpkg..." -ForegroundColor Cyan
& "$vcpkgDir\vcpkg.exe" install raylib imgui

# 3. Download latest rlImGui bridge headers and source
Write-Host "[3/3] Downloading latest rlImGui bridge files..." -ForegroundColor Cyan
$projectDir = Get-Location
Invoke-WebRequest -Uri "https://raw.githubusercontent.com/raylib-extras/rlImGui/main/rlImGui.h" -OutFile "$projectDir\rlImGui.h"
Invoke-WebRequest -Uri "https://raw.githubusercontent.com/raylib-extras/rlImGui/main/rlImGui.cpp" -OutFile "$projectDir\rlImGui.cpp"

Write-Host "=== Success! Open Visual Studio, add rlImGui.cpp to your project, and build. ===" -ForegroundColor Green