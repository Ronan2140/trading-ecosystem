import socket
import struct
import time

order_format = "<QQ8sIc3s"

def send_order(s, order_id, price, qty, side):
    # side : 'B' pour Bid (Achat), 'S' pour Ask (Vente)
    msg = struct.pack(order_format, order_id, price, b"AAPL", qty, side.encode(), b'\x00'*3)
    s.sendall(msg)
    print(f"Sent: {side} {qty} @ {price}")

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect(('127.0.0.1', 8080))

    send_order(s, 1, 105, 100, 'S')
    send_order(s, 2, 103, 50,  'S')

    time.sleep(0.1)

    send_order(s, 3, 110, 200, 'B')

print("Request sent")