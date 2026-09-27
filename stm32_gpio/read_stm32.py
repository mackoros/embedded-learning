import serial
import time

PORT = '/dev/ttyAMA0'
BAUD = 9600

try:
    ser = serial.Serial(PORT, BAUD, timeout=2)
    time.sleep(2)
    print(f"Порт {PORT} открыт. Ожидание данных...")
except serial.SerialException as e:
    print(f"Ошибка открытия порта: {e}")
    exit()

try:
    while True:
        line = ser.readline().decode('utf-8', errors='ignore').strip()
        if not line:
            continue
        if line.startswith("T:"):
            parts = line.split(',')
            temp = parts[0].split(':')[1]
            hum = parts[1].split(':')[1]
            print(f"Температура: {temp} °C, Влажность: {hum} %")
        else:
            print(f"Сообщение: {line}")
except KeyboardInterrupt:
    print("\nВыход.")
finally:
    ser.close()
