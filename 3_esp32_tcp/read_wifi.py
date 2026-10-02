import socket
import time
from RPLCD.i2c import CharLCD

ESP_HOST = '192.168.4.1'
ESP_PORT = 5000
LCD_ADDR = 0x27

lcd = CharLCD(i2c_expander='PCF8574', address=LCD_ADDR,
              port=1, cols=16, rows=2, charmap='A02')

def show(t, h):
    lcd.clear()
    lcd.cursor_pos = (0, 0); lcd.write_string(f"Temp: {t:>5} C")
    lcd.cursor_pos = (1, 0); lcd.write_string(f"Hum:  {h:>5} %")

lcd.clear(); lcd.write_string("Connecting...")

while True:
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((ESP_HOST, ESP_PORT))
        print(f"Connected to {ESP_HOST}:{ESP_PORT}")
        lcd.clear(); lcd.write_string("ESP32 connected")
        buf = b""
        while True:
            data = s.recv(128)
            if not data:
                raise ConnectionResetError
            buf += data
            while b"\n" in buf:
                line, buf = buf.split(b"\n", 1)
                line = line.decode('utf-8', errors='ignore').strip()
                if line.startswith("T:"):
                    parts = line.split(',')
                    t = float(parts[0].split(':')[1])
                    h = float(parts[1].split(':')[1])
                    print(f"T={t}  H={h}")
                    show(t, h)
    except (ConnectionResetError, OSError) as e:
        print(f"Disconnected: {e}. Retry in 3s...")
        lcd.clear(); lcd.write_string("ESP32 lost")
        time.sleep(3)
