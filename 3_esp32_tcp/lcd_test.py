from RPLCD.i2c import CharLCD
import time

lcd = CharLCD(
    i2c_expander='PCF8574',
    address=0x27,
    port=1,
    cols=16,
    rows=2,
    charmap='A02',
    auto_linebreaks=True,
)

lcd.clear()
lcd.cursor_pos = (0, 0)
lcd.write_string('Temp: 23 C')
lcd.cursor_pos = (1, 0)
lcd.write_string('Hum:  45 %')

while True:
    time.sleep(1)
