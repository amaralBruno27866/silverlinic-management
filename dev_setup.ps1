#!/usr/bin/env pwsh

<#
.SYNOPSIS
    Setup script for SilverClinic development environment
.DESCRIPTION
    Configures the development environment and provides quick access to common tasks
.PARAMETER Task
    The development task to perform: setup, build, test, run, clean, or help
.PARAMETER Configuration
    Build configuration: Debug or Release (default: Debug)
.EXAMPLE
    pwsh ./dev_setup.ps1 -Task setup
    pwsh ./dev_setup.ps1 -Task build
    pwsh ./dev_setup.ps1 -Task test
    pwsh ./dev_setup.ps1 -Task run
#>

param(
    [Parameter(Mandatory=$false)]
    [ValidateSet("setup", "build", "test", "run", "clean", "help")]
    [string]$Task = "help",
    
    [Parameter(Mandatory=$false)]
    [ValidateSet("Debug", "Release")]
    [string]$Configuration = "Debug"
)

# Colors for output
$ColorInfo = "Cyan"
$ColorSuccess = "Green"
$ColorWarning = "Yellow"
$ColorError = "Red"

function Write-Header {
    param([string]$Message)
    Write-Host "`n=== $Message ===" -ForegroundColor $ColorInfo
}

function Write-Success {
    param([string]$Message)
    Write-Host "✅ $Message" -ForegroundColor $ColorSuccess
}

function Write-Warning {
    param([string]$Message)
    Write-Host "⚠️  $Message" -ForegroundColor $ColorWarning
}

function Write-Error {
    param([string]$Message)
    Write-Host "❌ $Message" -ForegroundColor $ColorError
}

function Show-Help {
    Write-Header "SilverClinic Development Environment"
    Write-Host ""
    Write-Host "Available tasks:" -ForegroundColor $ColorInfo
    Write-Host "  setup  - Check dependencies and prepare environment" -ForegroundColor White
    Write-Host "  build  - Build the project (Debug or Release)" -ForegroundColor White
    Write-Host "  test   - Run all tests" -ForegroundColor White
    Write-Host "  run    - Build and run the application" -ForegroundColor White
    Write-Host "  clean  - Clean build directories" -ForegroundColor White
    Write-Host "  help   - Show this help message" -ForegroundColor White
    Write-Host ""
    Write-Host "Examples:" -ForegroundColor $ColorInfo
    Write-Host "  pwsh ./dev_setup.ps1 -Task setup" -ForegroundColor Yellow
    Write-Host "  pwsh ./dev_setup.ps1 -Task build -Configuration Release" -ForegroundColor Yellow
    Write-Host "  pwsh ./dev_setup.ps1 -Task test" -ForegroundColor Yellow
    Write-Host "  pwsh ./dev_setup.ps1 -Task run" -ForegroundColor Yellow
}

function Test-Dependencies {
    Write-Header "Checking Dependencies"
    
    # Check CMake
    try {
        $cmakeVersion = cmake --version | Select-Object -First 1
        Write-Success "CMake found: $cmakeVersion"
    }
    catch {
        Write-Error "CMake not found. Please install CMake."
        return $false
    }
    
    # Check GCC
    try {
        $gccVersion = g++ --version | Select-Object -First 1
        Write-Success "GCC found: $gccVersion"
    }
    catch {
        Write-Error "GCC not found. Please install build-essential."
        return $false
    }
    
    # Check SQLite
    try {
        $sqliteVersion = sqlite3 --version
        Write-Success "SQLite found: $sqliteVersion"
    }
    catch {
        Write-Warning "SQLite3 command line tool not found (optional for development)"
    }
    
    return $true
}

function Setup-Environment {
    Write-Header "Setting up Development Environment"
    
    if (-not (Test-Dependencies)) {
        Write-Error "Dependency check failed. Please install missing dependencies."
        return $false
    }
    
    # Create necessary directories
    $dirs = @("build", "build_test", "data", "data/pdfs")
    foreach ($dir in $dirs) {
        if (-not (Test-Path $dir)) {
            New-Item -ItemType Directory -Path $dir -Force | Out-Null
            Write-Success "Created directory: $dir"
        }
    }
    
    # Create .gitkeep files
    $gitkeepFiles = @("data/.gitkeep", "data/pdfs/.gitkeep")
    foreach ($file in $gitkeepFiles) {
        if (-not (Test-Path $file)) {
            New-Item -ItemType File -Path $file -Force | Out-Null
            Write-Success "Created: $file"
        }
    }
    
    Write-Success "Development environment setup complete!"
    return $true
}

function Build-Project {
    Write-Header "Building SilverClinic ($Configuration)"
    
    try {
        & pwsh -NoProfile -File ./build_windows.ps1 -Configuration $Configuration
        if ($LASTEXITCODE -eq 0) {
            Write-Success "Build completed successfully!"
            return $true
        } else {
            Write-Error "Build failed with exit code $LASTEXITCODE"
            return $false
        }
    }
    catch {
        Write-Error "Build script failed: $($_.Exception.Message)"
        return $false
    }
}

function Run-Tests {
    Write-Header "Running Tests"
    
    try {
        & pwsh -NoProfile -File ./run_tests_windows.ps1
        if ($LASTEXITCODE -eq 0) {
            Write-Success "All tests passed!"
            return $true
        } else {
            Write-Error "Tests failed with exit code $LASTEXITCODE"
            return $false
        }
    }
    catch {
        Write-Error "Test script failed: $($_.Exception.Message)"
        return $false
    }
}

function Run-Application {
    Write-Header "Building and Running SilverClinic"
    
    if (Build-Project) {
        try {
            & pwsh -NoProfile -File ./run_app.ps1 -Configuration $Configuration
            if ($LASTEXITCODE -eq 0) {
                Write-Success "Application executed successfully!"
                return $true
            } else {
                Write-Error "Application failed with exit code $LASTEXITCODE"
                return $false
            }
        }
        catch {
            Write-Error "Failed to run application: $($_.Exception.Message)"
            return $false
        }
    } else {
        Write-Error "Cannot run application - build failed"
        return $false
    }
}

function Clean-BuildDirs {
    Write-Header "Cleaning Build Directories"
    
    $buildDirs = @("build", "build_test")
    foreach ($dir in $buildDirs) {
        if (Test-Path $dir) {
            Remove-Item -Recurse -Force $dir
            Write-Success "Cleaned: $dir"
        }
    }
    
    # Remove compile_commands.json if it exists
    if (Test-Path "compile_commands.json") {
        Remove-Item "compile_commands.json"
        Write-Success "Removed: compile_commands.json"
    }
    
    Write-Success "Clean completed!"
}

# Main execution
try {
    switch ($Task.ToLower()) {
        "setup" { 
            Setup-Environment
        }
        "build" { 
            Build-Project
        }
        "test" { 
            Run-Tests
        }
        "run" { 
            Run-Application
        }
        "clean" { 
            Clean-BuildDirs
        }
        "help" { 
            Show-Help
        }
        default { 
            Write-Error "Unknown task: $Task"
            Show-Help
            exit 1
        }
    }
}
catch {
    Write-Error "Script failed: $($_.Exception.Message)"
    exit 1
}
