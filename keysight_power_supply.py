import pyvisa
import JSONoperators as js
#import Logging
#import datetime

def SendCommand(command, config="MainConfig"):

    address = js.ReadJSONConfig("keysight_power_supply","address")

    rm = pyvisa.ResourceManager()
    keysight = rm.open_resource(address)

    
    keysight.write(command)











#https://github.com/MarkDing/GPIB-pyvisa
