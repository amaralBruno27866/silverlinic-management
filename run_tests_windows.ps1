# PowerShell script to run all tests in Silver Clinic
# Compatible with pwsh (PowerShell 7+) and Windows PowerShell 5.1

param(
    [string]$Configuration = "Release",
    [string]$TestFilter = "*",
    [switch]$Verbose,
    [switch]$Help
)

if ($Help) {
    Write-Host "Silver Clinic Test Runner" -ForegroundColor Cyan
    Write-Host "Usage: .\run_tests_windows.ps1 [-Configuration <Debug|Release>] [-TestFilter <pattern>] [-Verbose] [-Help]" -ForegroundColor Green
    Write-Host ""
    Write-Host "Parameters:" -ForegroundColor Yellow
    Write-Host "  -Configuration  Build configuration (Debug or Release, default: Release)"
    Write-Host "  -TestFilter     Filter tests by name pattern (default: * for all tests)"
    Write-Host "  -Verbose        Show detailed test output"
    Write-Host "  -Help           Show this help message"
    Write-Host ""
    Write-Host "Examples:" -ForegroundColor Green
    Write-Host "  .\run_tests_windows.ps1                              # Run all tests"
    Write-Host "  .\run_tests_windows.ps1 -TestFilter beck*            # Run only Beck inventory tests"
    Write-Host "  .\run_tests_windows.ps1 -Configuration Debug -Verbose # Run debug tests with verbose output"
    exit 0
}

Write-Host "=== Silver Clinic - Test Runner ===" -ForegroundColor Cyan
Write-Host "PowerShell Version: $($PSVersionTable.PSVersion)" -ForegroundColor Gray
Write-Host "Configuration: $Configuration" -ForegroundColor Yellow
Write-Host "Test Filter: $TestFilter" -ForegroundColor Yellow
Write-Host ""

# Check if build directory exists
if (-not (Test-Path "build\$Configuration")) {
    Write-Host "[ERROR] Build directory not found: build\$Configuration" -ForegroundColor Red
    Write-Host "Please run .\build_windows.ps1 first" -ForegroundColor Yellow
    exit 1
}

# Find all test executables
$testPattern = "build\$Configuration\test_*.exe"
$testExecutables = Get-ChildItem -Path $testPattern -ErrorAction SilentlyContinue

if (-not $testExecutables) {
    Write-Host "[ERROR] No test executables found in build\$Configuration\" -ForegroundColor Red
    Write-Host "Available files:" -ForegroundColor Yellow
    Get-ChildItem "build\$Configuration\*.exe" | ForEach-Object { Write-Host "  $($_.Name)" -ForegroundColor Gray }
    exit 1
}

# Filter tests based on pattern
$filteredTests = $testExecutables | Where-Object { $_.BaseName -like $TestFilter }

if (-not $filteredTests) {
    Write-Host "[WARNING] No tests match filter: $TestFilter" -ForegroundColor Yellow
    Write-Host "Available tests:" -ForegroundColor Cyan
    $testExecutables | ForEach-Object { Write-Host "  $($_.BaseName)" -ForegroundColor Gray }
    exit 0
}

Write-Host "[INFO] Found $($filteredTests.Count) test(s) matching pattern '$TestFilter'" -ForegroundColor Green
Write-Host ""

# Run tests
$passedTests = 0
$failedTests = 0
$testResults = @()

foreach ($testExe in $filteredTests) {
    $testName = $testExe.BaseName
    Write-Host "🧪 Running: $testName" -ForegroundColor Cyan
    
    $startTime = Get-Date
    
    try {
        if ($Verbose) {
            & $testExe.FullName
            $exitCode = $LASTEXITCODE
        } else {
            $output = & $testExe.FullName 2>&1
            $exitCode = $LASTEXITCODE
        }
        
        $endTime = Get-Date
        $duration = ($endTime - $startTime).TotalSeconds
        
        if ($exitCode -eq 0) {
            Write-Host "   ✅ PASSED ($([math]::Round($duration, 2))s)" -ForegroundColor Green
            $passedTests++
            $testResults += @{
                Name = $testName
                Status = "PASSED"
                Duration = $duration
                Output = if (-not $Verbose) { $output } else { $null }
            }
        } else {
            Write-Host "   ❌ FAILED ($([math]::Round($duration, 2))s) - Exit Code: $exitCode" -ForegroundColor Red
            $failedTests++
            $testResults += @{
                Name = $testName
                Status = "FAILED"
                Duration = $duration
                ExitCode = $exitCode
                Output = if (-not $Verbose) { $output } else { $null }
            }
            
            if (-not $Verbose -and $output) {
                Write-Host "   Error Output:" -ForegroundColor Yellow
                $output | ForEach-Object { Write-Host "     $_" -ForegroundColor Gray }
            }
        }
    } catch {
        Write-Host "   💥 ERROR: $_" -ForegroundColor Red
        $failedTests++
        $testResults += @{
            Name = $testName
            Status = "ERROR"
            Error = $_.Exception.Message
        }
    }
    
    Write-Host ""
}

# Summary
Write-Host "=== TEST SUMMARY ===" -ForegroundColor Cyan
Write-Host "Total Tests: $($passedTests + $failedTests)" -ForegroundColor White
Write-Host "✅ Passed: $passedTests" -ForegroundColor Green
Write-Host "❌ Failed: $failedTests" -ForegroundColor Red

if ($failedTests -eq 0) {
    Write-Host "🎉 ALL TESTS PASSED!" -ForegroundColor Green
} else {
    Write-Host "⚠️  Some tests failed. Check output above for details." -ForegroundColor Yellow
}

Write-Host ""
Write-Host "Detailed Results:" -ForegroundColor Cyan
$testResults | ForEach-Object {
    $status = switch ($_.Status) {
        "PASSED" { "✅" }
        "FAILED" { "❌" }
        "ERROR" { "💥" }
    }
    $duration = if ($_.Duration) { " ($([math]::Round($_.Duration, 2))s)" } else { "" }
    Write-Host "  $status $($_.Name)$duration" -ForegroundColor Gray
}

# Exit with appropriate code
exit $failedTests
