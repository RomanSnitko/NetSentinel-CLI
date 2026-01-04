import socket
import threading
import time

PORTS = [8080, 8888, 7777, 9999, 6666]

def start_listener(port):
    try:
        server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        server.bind(('0.0.0.0', port))
        server.listen(5)
        time.sleep(60)
        server.close()
    except Exception: pass

if __name__ == "__main__":
    print(f"[*] Opening ports {PORTS} for 60 seconds...")
    for p in PORTS:
        threading.Thread(target=start_listener, args=(p,)).start()