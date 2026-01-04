DB_NAME="netsentinel_db"
DB_USER="roman"
DB_PASS="31072007"
INIT_SQL="../sql/init.sql"

echo "[*] Starting PostgreSQL setup for NetSentinel..."

if ! command -v psql &> /dev/null
then
    echo "[!] Error: PostgreSQL is not installed. Install it with: sudo apt install postgresql"
    exit 1
fi

echo "[*] Creating database user: $DB_USER..."
sudo -u postgres psql -t -Ac "SELECT 1 FROM pg_roles WHERE rolname='$DB_USER'" | grep -q 1 || \
sudo -u postgres psql -c "CREATE USER $DB_USER WITH PASSWORD '$DB_PASS';"

echo "[*] Creating database: $DB_NAME..."
sudo -u postgres psql -lqt | cut -d \| -f 1 | grep -qw $DB_NAME || \
sudo -u postgres psql -c "CREATE DATABASE $DB_NAME OWNER $DB_USER;"

echo "[*] Granting privileges..."
sudo -u postgres psql -c "GRANT ALL PRIVILEGES ON DATABASE $DB_NAME TO $DB_USER;"

if [ -f "$INIT_SQL" ]; then
    echo "[*] Initializing tables from $INIT_SQL..."

    sudo -u postgres psql -d $DB_NAME -f "$INIT_SQL"

    sudo -u postgres psql -d $DB_NAME -c "GRANT ALL ON SCHEMA public TO $DB_USER;"
else
    echo "[!] Warning: $INIT_SQL not found. Skipping table initialization."
fi

echo "[+] Database setup complete!"

  