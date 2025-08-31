<#
PowerShell helper to export duplicate detection queries into CSV files.
Usage:
  ./export_duplicates.ps1 -DbPath ./data/silverclinic.db -OutDir ./duplicates_report
#>
param(
    [string] $DbPath = "./data/test_database.db",
    [string] $OutDir = "./duplicates_report"
)

if (-not (Test-Path $DbPath)) {
    Write-Error "Database file not found: $DbPath"; exit 2
}

if (-not (Test-Path $OutDir)) {
    New-Item -Path $OutDir -ItemType Directory | Out-Null
}

# Read the SQL file
$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Definition
$sqlFile = Join-Path $scriptDir "duplicates_queries.sql"
if (-not (Test-Path $sqlFile)) { Write-Error "SQL file not found: $sqlFile"; exit 3 }
$sql = Get-Content $sqlFile -Raw

# Split queries by semicolon followed by newline (simple heuristic)
$queries = $sql -split "\n\n-- " | Where-Object { $_ -match '\S' }

# We'll run each block individually by detecting FROM table name
# Helper: run sqlite3 and capture CSV output
function Run-QueryToCsv([string]$query, [string]$outPath) {
    $tmpSql = [IO.Path]::GetTempFileName()
    Set-Content -Path $tmpSql -Value $query
    $cmd = "sqlite3 -header -csv `"$DbPath`" < `"$tmpSql`" > `"$outPath`""
    $proc = Start-Process -FilePath pwsh -ArgumentList "-Command", $cmd -NoNewWindow -Wait -PassThru
    Remove-Item $tmpSql -ErrorAction SilentlyContinue
}

# Execute named queries and write outputs
# 1) Clients by normalized email
$q1 = Get-Content $sqlFile -Raw -Encoding UTF8 | Select-String -Pattern "-- Clients by normalized email" -Context 0,10
$qblock1 = ($sql -split "-- Clients by normalized email")[1] -split "-- Clients by firstname" | Select-Object -First 1
$qblock1 = """$qblock1"""
Run-QueryToCsv $qblock1 (Join-Path $OutDir "clients_by_normalized_email.csv")

# 2) Clients by firstname+lastname+phone
$qblock2 = ($sql -split "-- Clients by firstname+lastname+normalized_phone")[1] -split "-- Assessors by normalized email" | Select-Object -First 1
$qblock2 = """$qblock2"""
Run-QueryToCsv $qblock2 (Join-Path $OutDir "clients_by_name_phone.csv")

# 3) Assessors by normalized email
$qblock3 = ($sql -split "-- Assessors by normalized email")[1] -split "-- Assessors by firstname+lastname+normalized_phone" | Select-Object -First 1
$qblock3 = """$qblock3"""
Run-QueryToCsv $qblock3 (Join-Path $OutDir "assessors_by_normalized_email.csv")

# 4) Assessors by firstname+lastname+phone
$qblock4 = ($sql -split "-- Assessors by firstname+lastname+normalized_phone")[1]
$qblock4 = """$qblock4"""
Run-QueryToCsv $qblock4 (Join-Path $OutDir "assessors_by_name_phone.csv")

Write-Output "Exports written to: $OutDir"
