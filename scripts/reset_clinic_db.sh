#!/usr/bin/env bash
# Backup existing clinic.db and replace with a fresh empty DB using reset_clinic_db.sql
DB_DIR="./data"
DB_FILE="$DB_DIR/clinic.db"
BACKUP_DIR="$DB_DIR/backups"
SQL_SCHEMA="./scripts/reset_clinic_db.sql"

set -euo pipefail

if [ ! -f "$SQL_SCHEMA" ]; then
  echo "Schema file not found: $SQL_SCHEMA" >&2
  exit 2
fi

mkdir -p "$BACKUP_DIR"

if [ -f "$DB_FILE" ]; then
  TIMESTAMP=$(date +"%Y%m%d_%H%M%S")
  cp "$DB_FILE" "$BACKUP_DIR/clinic.db.$TIMESTAMP.bak"
  echo "Backup created: $BACKUP_DIR/clinic.db.$TIMESTAMP.bak"
  rm -f "$DB_FILE"
fi

# create new sqlite DB and apply schema
sqlite3 "$DB_FILE" < "$SQL_SCHEMA"
chmod 664 "$DB_FILE"

echo "New clinic.db created at $DB_FILE"
echo "You can now start the GUI and it will use a clean database."
