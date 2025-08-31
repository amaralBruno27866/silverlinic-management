#!/usr/bin/env bash
# Run duplicate export using sqlite3 and the provided SQL file.
DB_PATH="./data/test_database.db"
OUT_DIR="./duplicates_report"

# Prefer clinic.db if present
if [ -f "./data/clinic.db" ]; then
	DB_PATH="./data/clinic.db"
fi
SQL_FILE="./scripts/duplicates_queries.sql"

mkdir -p "$OUT_DIR"

# Split the SQL file into named parts and run each
awk '/-- Clients by normalized email/{flag=1;next}/-- Clients by firstname\+lastname\+normalized_phone/{flag=0}flag{print > "./tmp_clients_by_email.sql"}' "$SQL_FILE"
awk '/-- Clients by firstname\+lastname\+normalized_phone/{flag=1;next}/-- Assessors by normalized email/{flag=0}flag{print > "./tmp_clients_by_name_phone.sql"}' "$SQL_FILE"
awk '/-- Assessors by normalized email/{flag=1;next}/-- Assessors by firstname\+lastname\+normalized_phone/{flag=0}flag{print > "./tmp_assessors_by_email.sql"}' "$SQL_FILE"
awk '/-- Assessors by firstname\+lastname\+normalized_phone/{flag=1;next}flag{print > "./tmp_assessors_by_name_phone.sql"}' "$SQL_FILE"

sqlite3 -header -csv "$DB_PATH" < ./tmp_clients_by_email.sql > "$OUT_DIR/clients_by_normalized_email.csv" || echo "clients_by_normalized_email failed"
sqlite3 -header -csv "$DB_PATH" < ./tmp_clients_by_name_phone.sql > "$OUT_DIR/clients_by_name_phone.csv" || echo "clients_by_name_phone failed"
sqlite3 -header -csv "$DB_PATH" < ./tmp_assessors_by_email.sql > "$OUT_DIR/assessors_by_normalized_email.csv" || echo "assessors_by_normalized_email failed"
sqlite3 -header -csv "$DB_PATH" < ./tmp_assessors_by_name_phone.sql > "$OUT_DIR/assessors_by_name_phone.csv" || echo "assessors_by_name_phone failed"

rm -f ./tmp_*.sql

echo "Exports written to: $OUT_DIR"
