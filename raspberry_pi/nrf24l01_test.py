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

try:
    count = 0
    print(f'Receive from {address}')
    while True:

            # As long as data is ready for processing, process it.
            while nrf.data_ready():
                # Count message and record time of reception.            
                count += 1
                now = datetime.now()
                
                # Read pipe and payload for message.
                pipe = nrf.data_pipe()
                payload = nrf.get_payload()    

                # Resolve protocol number.
                protocol = payload[0] if len(payload) > 0 else -1            

                hex = ':'.join(f'{i:02x}' for i in payload)

                # Show message received as hex.
                print(f"{now:%Y-%m-%d %H:%M:%S.%f}: pipe: {pipe}, len: {len(payload)}, bytes: {hex}, count: {count}")

                # If the length of the message is 9 bytes and the first byte is 0x01, then we try to interpret the bytes
                # sent as an example message holding a temperature and humidity sent from the "simple-sender.py" program.
                if len(payload) == 32 and payload[0] == 0x01:
                    values = struct.unpack("<Bff", payload)
                    print(f'Protocol: {values[0]}, temperature: {values[1]}, humidity: {values[2]}')
                
            # Sleep 100 ms.
            time.sleep(0.1)
except:
    traceback.print_exc()
    nrf.power_down()
    pi.stop()

