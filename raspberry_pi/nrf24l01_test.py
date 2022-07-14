import pigpio
from nrf24 import *

hostname = "localhost"
port = 8888

#Connect to raspberry
pi=pigpio.pi(hostname, port)