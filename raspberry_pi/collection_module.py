from fileinput import filename
import time
import sys
import struct
import datetime
import traceback
import pigpio
import csv
from nrf24 import *


nRF_address = '1SnSR'
filename = "field_measurements.csv"
ce_pin=25
nRF_channel=100

my_pi=pigpio.pi()
if not my_pi.connected:
    print("Could not cominicate with pins")
    sys.exit()
antena =NRF24(my_pi, ce=ce_pin, payload_size=RF24_PAYLOAD.DYNAMIC, channel=nRF_channel,data_rate=RF24_DATA_RATE.RATE_250KBPS,pa_level=RF24_PA.HIGH)
antena.set_address_bytes(len(nRF_address))
antena.open_reading_pipe(RF24_RX_ADDR.P1, nRF_address)
antena.show_registers()

try:
    print(f'Message using address: {nRF_address}')
    count = 0
    while True:
        while antena.data_ready():
            now=datetime.now()
            measured_time= datetime.now().strftime("%Y-%m-%d %H:%M:%S")
            pipe = antena.data_pipe()
            payload = antena.get_payload()    

                # Resolve protocol number.
            protocol = payload[0] if len(payload) > 0 else -1            

            hex = ':'.join(f'{i:02x}' for i in payload)

            # Show message received as hex.
            print(f"{now:%Y-%m-%d %H:%M:%S.%f}: pipe: {pipe}, len: {len(payload)}, bytes: {hex}, count: {count}")

            # If the length of the message is 9 bytes and the first byte is 0x01, then we try to interpret the bytes
            # sent as an example message holding a temperature and humidity sent from the "simple-sender.py" program.
            #if len(payload) == 9 and payload[0] == 0x01:
            if payload[0] == 0x01:
                values = struct.unpack("<BBBffhhh", payload)
                print(f'Module type: {values[0]}, module type: {values[1:3]}, temperature: {values[3]}, humidity: {values[4]}, soil: {values[5]},light: {values[6]}, valve {values[7]}')
                with open(filename, 'a', newline='') as file:
                    typer= csv.writer(file)
                    typer.writerow([measured_time,values[0],values[1]+values[2],round(values[3],2),round(values[4],2),values[5],values[6],values[7],'','','','',''])
                    file.close()
            if payload[0] ==0x02:
                values = struct.unpack("<BBBhhhff", payload)
                print(f'Module number: {values[0]}, module type: {values[1:3]}, nitrogen: {values[5]},phosphorus: {values[6]}, potasium {values[7]}, latitude: {values[3]}, longitude: {values[4]}')
                with open(filename, 'a', newline='') as file:
                    typer= csv.writer(file)
                    typer.writerow([measured_time,values[0],values[1]+values[2],'','','','','',values[5],values[6],values[7],round(values[3],5),round(values[4],5)])
                    file.close()            
        # Sleep 100 ms.
        time.sleep(0.1)
except:
    traceback.print_exc()
    antena.power_down()
    my_pi.stop()


