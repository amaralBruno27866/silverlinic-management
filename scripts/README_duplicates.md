Duplicate detection scripts

Files:
- `scripts/duplicates_queries.sql` - SQL queries that detect duplicate clients and assessors by normalized email and by firstname+lastname+phone.
- `scripts/export_duplicates.ps1` - PowerShell script to run the queries and export CSVs.
- `scripts/export_duplicates.sh` - Bash script alternative to run the queries and export CSVs.

Usage examples:
PowerShell (Windows or pwsh):

```powershell
pwsh ./scripts/export_duplicates.ps1 -DbPath ./data/test_database.db -OutDir ./duplicates_report
```

Bash (Linux/macOS):

```bash
./scripts/export_duplicates.sh
```

The scripts expect `sqlite3` available on PATH and a local database file (default points to `./data/test_database.db`). The output CSVs are written to `./duplicates_report/`.
