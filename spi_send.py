import spidev
import time

spi = spidev.SpiDev()
spi.open(0, 0)  # Bus 0, Device 0 (CE0)
spi.mode = 0
spi.max_speed_hz = 500000

def send_message(msg):
    spi.xfer2(list(msg.encode('ascii')))
    time.sleep(0.01)

while True:
    message = "$PH,0001234567,0123,0075#"
    send_message(message)
    print("Sent:", message)
    time.sleep(1)
