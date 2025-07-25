# PowerShell Core build script for Silver Clinic
# Compatible with pwsh (PowerShell 7+) and Windows PowerShell 5.1

param(
    [string]$Configuration = "Release",
    [switch]$Clean,
    [switch]$Help
)

if ($Help) {
    Write-Host "Silver Clinic Build Script" -ForegroundColor Cyan
    Write-Host "Usage: .\build_windows.ps1 [-Configuration <Debug|Release>] [-Clean] [-Help]" -ForegroundColor Green
    Write-Host ""
    Write-Host "Parameters:" -ForegroundColor Yellow
    Write-Host "  -Configuration  Build configuration (Debug or Release, default: Release)"
    Write-Host "  -Clean         Clean build directory before building"
    Write-Host "  -Help          Show this help message"
    Write-Host ""
    Write-Host "Examples:" -ForegroundColor Green
    Write-Host "  .\build_windows.ps1                    # Build Release version"
    Write-Host "  .\build_windows.ps1 -Configuration Debug  # Build Debug version"
    Write-Host "  .\build_windows.ps1 -Clean               # Clean and build"
    exit 0
}

Write-Host "=== Silver Clinic - PowerShell Build Script ===" -ForegroundColor Cyan
Write-Host "PowerShell Version: $($PSVersionTable.PSVersion)" -ForegroundColor Gray
Write-Host "Configuration: $Configuration" -ForegroundColor Yellow

# Clean build directory if requested
if ($Clean -and (Test-Path "build")) {
    Write-Host "[INFO] Cleaning build directory..." -ForegroundColor Yellow
    Remove-Item -Recurse -Force "build"
}

# Check if vcpkg is available
$vcpkgPath = Get-Command vcpkg -ErrorAction SilentlyContinue
if (-not $vcpkgPath) {
    Write-Host "[WARNING] vcpkg not found in PATH" -ForegroundColor Yellow
    Write-Host "For automatic dependency management, install vcpkg:" -ForegroundColor Cyan
    Write-Host "  git clone https://github.com/Microsoft/vcpkg.git C:\vcpkg" -ForegroundColor Gray
    Write-Host "  cd C:\vcpkg" -ForegroundColor Gray
    Write-Host "  .\bootstrap-vcpkg.bat" -ForegroundColor Gray
    Write-Host "  .\vcpkg integrate install" -ForegroundColor Gray
    Write-Host "  .\vcpkg install sqlite3:x64-windows libharu:x64-windows" -ForegroundColor Gray
    Write-Host ""
} else {
    Write-Host "[INFO] vcpkg found at: $($vcpkgPath.Source)" -ForegroundColor Green
}

# Create build directory
if (-not (Test-Path "build")) {
    New-Item -ItemType Directory -Name "build" | Out-Null
    Write-Host "[INFO] Created build directory" -ForegroundColor Green
}

Set-Location "build"

# Configure with CMake
Write-Host "[INFO] Configuring project with CMake..." -ForegroundColor Green

# Detect operating system and set appropriate generator
$isWindowsOS = $PSVersionTable.Platform -eq "Win32NT" -or $env:OS -eq "Windows_NT"
$isLinuxOS = $PSVersionTable.Platform -eq "Unix" -and $env:USER

if ($isWindowsOS) {
    Write-Host "[INFO] Detected Windows environment - using Visual Studio generator" -ForegroundColor Green
    $cmakeArgs = @(
        "-G", "Visual Studio 17 2022",
        "-A", "x64"
    )
} else {
    Write-Host "[INFO] Detected Linux environment - using Unix Makefiles generator" -ForegroundColor Green
    $cmakeArgs = @(
        "-G", "Unix Makefiles",
        "-DCMAKE_BUILD_TYPE=$Configuration"
    )
}

# Add common CMake arguments
$cmakeArgs += "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON"

# Add vcpkg toolchain if available
if ($env:VCPKG_ROOT) {
    $cmakeArgs += "-DCMAKE_TOOLCHAIN_FILE=$env:VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"
    Write-Host "[INFO] Using vcpkg toolchain from: $env:VCPKG_ROOT" -ForegroundColor Green
}

$cmakeArgs += ".."

try {
    & cmake @cmakeArgs
    if ($LASTEXITCODE -ne 0) {
        throw "CMake configuration failed"
    }
} catch {
    Write-Host "[ERROR] CMake configuration failed: $_" -ForegroundColor Red
    if ($env:VCPKG_ROOT) {
        Write-Host "[INFO] Trying without vcpkg..." -ForegroundColor Yellow
        try {
            if ($isWindowsOS) {
                & cmake -G "Visual Studio 17 2022" -A x64 -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
            } else {
                & cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=$Configuration -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
            }
            if ($LASTEXITCODE -ne 0) {
                throw "CMake configuration failed completely"
            }
        } catch {
            Write-Host "[ERROR] CMake configuration failed completely: $_" -ForegroundColor Red
            Set-Location ".."
            exit 1
        }
    } else {
        Set-Location ".."
        exit 1
    }
}

# Build the project
Write-Host "[INFO] Building project ($Configuration)..." -ForegroundColor Green
try {
    if ($isWindowsOS) {
        & cmake --build . --config $Configuration --parallel
    } else {
        & cmake --build . --parallel
    }
    if ($LASTEXITCODE -ne 0) {
        throw "Build failed"
    }
} catch {
    Write-Host "[ERROR] Build failed: $_" -ForegroundColor Red
    Set-Location ".."
    exit 1
}

Write-Host "[SUCCESS] Build completed successfully!" -ForegroundColor Green
Write-Host ""
Write-Host "📁 Build Results:" -ForegroundColor Cyan

if ($isWindowsOS) {
    Write-Host "  Build directory: build/$Configuration/" -ForegroundColor Gray
    Write-Host "  Main application: build/$Configuration/SilverClinic.exe" -ForegroundColor Gray
    Write-Host "  Test executables: build/$Configuration/test_*.exe" -ForegroundColor Gray
    Write-Host "  Example demos: build/$Configuration/*_demo.exe" -ForegroundColor Gray
    
    # Check if executables were created
    $mainExe = "$Configuration/SilverClinic.exe"
    if (Test-Path $mainExe) {
        Write-Host "✅ Main application ready to run" -ForegroundColor Green
        Write-Host "   To execute: .\build\$Configuration\SilverClinic.exe" -ForegroundColor Gray
    }
} else {
    Write-Host "  Build directory: build/" -ForegroundColor Gray
    Write-Host "  Main application: build/SilverClinic" -ForegroundColor Gray
    Write-Host "  Test executables: build/test_*" -ForegroundColor Gray
    Write-Host "  Example demos: build/*_demo" -ForegroundColor Gray
    
    # Check if executables were created
    $mainExe = "SilverClinic"
    if (Test-Path $mainExe) {
        Write-Host "✅ Main application ready to run" -ForegroundColor Green
        Write-Host "   To execute: ./build/SilverClinic" -ForegroundColor Gray
    } else {
        Write-Host "⚠️  Main executable not found at expected location" -ForegroundColor Yellow
    }
}

Set-Location ".."

Write-Host ""
Write-Host "🎉 Build process completed!" -ForegroundColor Green
