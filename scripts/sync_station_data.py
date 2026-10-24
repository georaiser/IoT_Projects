"""
IoT Environmental Station — Automated Data Sync & Backup Tool
=============================================================
Architecture: Standalone Python CLI utility (Zero external pip dependencies).
Purpose:
  1. Queries the ESP32 REST API (/api/sensors and /api/history.csv).
  2. Merges newly acquired samples into a master cumulative CSV archive.
  3. Deduplicates records by timestamp to ensure data integrity.
  4. Displays a real-time terminal telemetry summary.

Usage:
  python sync_station_data.py
  python sync_station_data.py --ip 192.168.1.5
  python sync_station_data.py --output D:/00_IoT_Projects/data/master_history.csv
"""

import sys
import os
import csv
import json
import argparse
import urllib.request
import urllib.error
from datetime import datetime

DEFAULT_STATION_IP = "192.168.1.5"
DEFAULT_TIMEOUT_SEC = 6
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
DEFAULT_DATA_DIR = os.path.normpath(os.path.join(SCRIPT_DIR, "..", "data"))
DEFAULT_MASTER_CSV = os.path.join(DEFAULT_DATA_DIR, "env_station_master.csv")


def fetch_url(url, timeout=DEFAULT_TIMEOUT_SEC):
    """Fetch URL content with proper user-agent and error handling."""
    req = urllib.request.Request(
        url,
        headers={"User-Agent": "EnvStationSync/1.0 (ESP32 Datalogger Client)"}
    )
    with urllib.request.urlopen(req, timeout=timeout) as response:
        return response.read().decode("utf-8", errors="replace")


def get_current_telemetry(base_url):
    """Retrieve real-time snapshot from /api/sensors."""
    url = f"{base_url}/api/sensors"
    try:
        raw = fetch_url(url)
        return json.loads(raw)
    except Exception as e:
        print(f"[!] Warning: Could not fetch real-time telemetry from {url}: {e}")
        return None


def fetch_history_csv(base_url):
    """Retrieve the full historical CSV datalog from /api/history.csv."""
    url = f"{base_url}/api/history.csv"
    try:
        raw_csv = fetch_url(url)
        lines = [line.strip() for line in raw_csv.splitlines() if line.strip()]
        if not lines:
            return []
        
        # Parse CSV
        reader = csv.reader(lines)
        header = next(reader, None)
        rows = [row for row in reader if len(row) >= 5]
        return rows
    except Exception as e:
        print(f"[!] Error: Failed to download CSV history from {url}: {e}")
        return []


def merge_into_master_csv(master_path, new_rows):
    """
    Merge new records into the master CSV file, eliminating duplicates by timestamp.
    Returns (added_count, total_count).
    """
    os.makedirs(os.path.dirname(master_path), exist_ok=True)
    existing_records = {}
    csv_header = ["timestamp", "temp_c", "humidity_pct", "pressure_hpa", "air_ppm"]

    # 1. Read existing master records if file exists
    if os.path.isfile(master_path):
        try:
            with open(master_path, mode="r", encoding="utf-8", newline="") as f:
                reader = csv.reader(f)
                header = next(reader, None)
                for row in reader:
                    if len(row) >= 5:
                        ts = row[0].strip()
                        existing_records[ts] = row
        except Exception as e:
            print(f"[!] Warning reading existing master CSV: {e}")

    initial_count = len(existing_records)

    # 2. Add incoming rows (deduplicated by timestamp)
    added_count = 0
    for row in new_rows:
        ts = row[0].strip()
        # Validate timestamp format (e.g. YYYY-MM-DD HH:MM:SS)
        if len(ts) >= 16 and not ts.startswith("Sync") and not ts.startswith("Up"):
            if ts not in existing_records:
                existing_records[ts] = row
                added_count += 1

    total_count = len(existing_records)

    # 3. Sort chronologically by timestamp
    sorted_records = sorted(existing_records.values(), key=lambda r: r[0])

    # 4. Write back to master CSV
    with open(master_path, mode="w", encoding="utf-8", newline="") as f:
        writer = csv.writer(f)
        writer.writerow(csv_header)
        writer.writerows(sorted_records)

    return added_count, total_count


def display_telemetry_summary(data):
    """Print an elegant formatted terminal summary of station status."""
    if not data:
        return
    cur = data.get("current", {})
    st = data.get("stats", {})
    ts = data.get("timestamp", "N/A")
    uptime_s = data.get("uptime_s", 0)
    uptime_h = uptime_s / 3600.0

    print("=" * 64)
    print(f"  🌱 ESP32 ENVIRONMENTAL STATION — LIVE TELEMETRY")
    print(f"  🕒 Timestamp : {ts}  |  Uptime: {uptime_h:.1f} hours ({uptime_s}s)")
    print("=" * 64)
    print(f"  • Temperature (AHT20) : {cur.get('temp_aht', 0.0):.1f} °C   "
          f"[Min: {st.get('temp',{}).get('min', 0):.1f} | Avg: {st.get('temp',{}).get('avg', 0):.1f} | Max: {st.get('temp',{}).get('max', 0):.1f}]")
    print(f"  • Humidity (AHT20)    : {cur.get('humidity', 0.0):.1f} %    "
          f"[Min: {st.get('hum',{}).get('min', 0):.0f}% | Avg: {st.get('hum',{}).get('avg', 0):.0f}% | Max: {st.get('hum',{}).get('max', 0):.0f}%]")
    print(f"  • Pressure (BMP280)   : {cur.get('pressure', 0.0):.1f} hPa  "
          f"[Min: {st.get('pres',{}).get('min', 0):.1f} | Avg: {st.get('pres',{}).get('avg', 0):.1f} | Max: {st.get('pres',{}).get('max', 0):.1f}]")
    print(f"  • Air Quality (MQ135) : {cur.get('air_ppm', 0)} ppm [{cur.get('air_status', 'N/A')}]  "
          f"(Voltage: {cur.get('mq135_v', 0):.2f} V, ADC: {cur.get('mq135_raw', 0)})")
    
    stg = data.get("storage", {})
    if stg:
        warn_str = " ⚠️ [WARNING: MEMORY LOW!]" if stg.get("warning") else ""
        print(f"  • Flash Storage       : {stg.get('used_kb', 0):.1f} KB / {stg.get('total_kb', 512):.0f} KB "
              f"({stg.get('pct', 0):.1f}% used, ~{stg.get('days_left', 0)} days left){warn_str}")
    print("=" * 64)


def purge_device_storage(base_url):
    """Safely clear Flash history on ESP32 after successful backup."""
    url = f"{base_url}/api/history/clear?confirm=true"
    try:
        req = urllib.request.Request(
            url,
            headers={"User-Agent": "EnvStationSync/1.0"}
        )
        with urllib.request.urlopen(req, timeout=DEFAULT_TIMEOUT_SEC) as response:
            res_body = response.read().decode("utf-8")
            print(f"[✓] PURGE CONFIRMED: Flash memory on ESP32 successfully cleared.")
            return True
    except Exception as e:
        print(f"[!] Warning: Could not purge ESP32 storage: {e}")
        return False


def main():
    parser = argparse.ArgumentParser(description="Sync and backup telemetry from ESP32 Environmental Station.")
    parser.add_argument("--ip", default=DEFAULT_STATION_IP, help=f"ESP32 IP address or hostname (default: {DEFAULT_STATION_IP})")
    parser.add_argument("--output", default=DEFAULT_MASTER_CSV, help=f"Path to master CSV file (default: {DEFAULT_MASTER_CSV})")
    parser.add_argument("--purge", action="store_true", help="Download and backup all data to master CSV first, then safely purge Flash memory on ESP32.")
    args = parser.parse_args()

    base_url = f"http://{args.ip}"
    print(f"\n[+] Connecting to Environmental Station at {base_url}...")

    # 1. Fetch current status
    telemetry = get_current_telemetry(base_url)
    if telemetry:
        display_telemetry_summary(telemetry)

    # 2. Fetch history CSV
    print(f"[+] Requesting historical datalog from {base_url}/api/history.csv...")
    downloaded_rows = fetch_history_csv(base_url)
    print(f"[+] Received {len(downloaded_rows)} samples from device.")

    if not downloaded_rows:
        print("[!] No data rows retrieved. Check station connectivity.")
        return 1

    # 3. Merge into master CSV
    print(f"[+] Merging into master archive: {args.output}")
    added, total = merge_into_master_csv(args.output, downloaded_rows)
    print(f"[✓] SUCCESS: Added {added} new records. Master archive now contains {total} cumulative samples.")
    print(f"[✓] File location: {os.path.abspath(args.output)}")

    # 4. If --purge was requested, clear ESP32 memory now that data is safe on PC
    if args.purge:
        print(f"\n[+] --purge requested: Safely clearing device storage after verified backup...")
        purge_device_storage(base_url)

    print()
    return 0


if __name__ == "__main__":
    sys.exit(main())
