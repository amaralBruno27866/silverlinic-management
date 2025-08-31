param(
    [string]$DbDir = "./data",
    [string]$DbFile = "clinic.db"
)
$fullPath = Join-Path $DbDir $DbFile
$backupDir = Join-Path $DbDir "backups"
$schema = Join-Path (Split-Path -Parent $MyInvocation.MyCommand.Definition) "reset_clinic_db.sql"

if (-not (Test-Path $schema)) { Write-Error "Schema file missing: $schema"; exit 2 }

if (-not (Test-Path $backupDir)) { New-Item -Path $backupDir -ItemType Directory | Out-Null }

if (Test-Path $fullPath) {
    $ts = Get-Date -Format yyyyMMdd_HHmmss
    $bak = Join-Path $backupDir "clinic.db.$ts.bak"
    Copy-Item -Path $fullPath -Destination $bak
    Write-Output "Backup created: $bak"
    Remove-Item -Path $fullPath
}

# Create new DB from schema
& sqlite3 "$fullPath" < "$schema"
Write-Output "New clinic.db created at: $fullPath"
