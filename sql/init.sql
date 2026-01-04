CREATE TABLE IF NOT EXISTS devices (
    id SERIAL PRIMARY KEY,
    mac_address TEXT UNIQUE NOT NULL,
    ip_address TEXT,
    hostname TEXT,
    last_seen TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS network_events (
    id SERIAL PRIMARY KEY,
    mac_address TEXT,
    event_type TEXT,
    details TEXT,
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS metrics (
    id SERIAL PRIMARY KEY,
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    latency_ms FLOAT,
    jitter_ms FLOAT,
    download_speed_mbps FLOAT
);