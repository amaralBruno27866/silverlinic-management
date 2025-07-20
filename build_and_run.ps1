# Silver Clinic - Build and Run Script (PowerShell)
# ==========================================

param(
    [switch]$Clean,
    [switch]$BuildOnly,
    [switch]$RunOnly
)

# Set variables
$BUILD_DIR = "build_test"
$CMAKE_PATH = "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
$PROJECT_NAME = "SilverClinic"

Write-Host "==========================================" -ForegroundColor Cyan
Write-Host "   Silver Clinic - Build and Run Script" -ForegroundColor Cyan
Write-Host "==========================================" -ForegroundColor Cyan

if (-not $RunOnly) {
    # Clean previous build
    Write-Host "`n[1/4] Cleaning previous build..." -ForegroundColor Yellow
    if (Test-Path $BUILD_DIR) {
        Remove-Item $BUILD_DIR -Recurse -Force
        Write-Host "Previous build cleaned." -ForegroundColor Green
    } else {
        Write-Host "No previous build found." -ForegroundColor Gray
    }

    # Configure with CMake
    Write-Host "`n[2/4] Configuring project with CMake..." -ForegroundColor Yellow
    & $CMAKE_PATH -S . -B $BUILD_DIR
    if ($LASTEXITCODE -ne 0) {
        Write-Host "ERROR: CMake configuration failed!" -ForegroundColor Red
        Read-Host "Press Enter to exit"
        exit 1
    }

    # Build project
    Write-Host "`n[3/4] Building project..." -ForegroundColor Yellow
    & $CMAKE_PATH --build $BUILD_DIR
    if ($LASTEXITCODE -ne 0) {
        Write-Host "ERROR: Build failed!" -ForegroundColor Red
        Read-Host "Press Enter to exit"
        exit 1
    }
}

if (-not $BuildOnly) {
    # Run application
    Write-Host "`n[4/4] Running application..." -ForegroundColor Yellow
    Write-Host "==========================================" -ForegroundColor Cyan
    
    $EXE_PATH = "$BUILD_DIR\Debug\$PROJECT_NAME.exe"
    if (Test-Path $EXE_PATH) {
        & $EXE_PATH
    } else {
        Write-Host "ERROR: Executable not found at $EXE_PATH" -ForegroundColor Red
        Write-Host "Make sure the build completed successfully." -ForegroundColor Red
        Read-Host "Press Enter to exit"
        exit 1
    }
}

Write-Host "`n==========================================" -ForegroundColor Cyan
Write-Host "Build and execution completed!" -ForegroundColor Green
Write-Host "==========================================" -ForegroundColor Cyan
