import pigpio
from nrf24 import *

hostname = "localhost"
port = 8888
address = "AXRR1"
ce_pin=25

#Connect to raspberry
pi=pigpio.pi(hostname, port)
nrf=NRF24(pi,ce=ce_pin, payload_size=32,channel=100, data_rate=RF24_DATA_RATE.RATE_250KBPS, pa_level=RF24_PA.MIN)
nrf.set_address_bytes(len(address))

nrf.open_reading_pipe(RF24_RX_ADDR.P1, address)

nrf.show_registers()