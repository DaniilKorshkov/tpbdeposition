import pyvisa
import JSONoperators as js
#import Logging
#import datetime

def SendCommand(command, config="MainConfig"):

    address = js.ReadJSONConfig("keysight_power_supply","address")

    rm = pyvisa.ResourceManager()
    keysight = rm.open_resource(address)

    
    keysight.write(command)

def SendQuery(command, config="MainConfig"):

    address = js.ReadJSONConfig("keysight_power_supply","address")

    rm = pyvisa.ResourceManager()
    keysight = rm.open_resource(address)

    
    ret = keysight.query_ascii_values(command)

    return(ret)



def ReadVoltage(channel):
    SendCommand("*RST")
    SendCommand(f"INSTR CH{channel}")
    ret = SendQuery("VOLT?")
    return ret


def ReadAmperage(channel):
    SendCommand("*RST")
    SendCommand(f"INSTR CH{channel}")
    ret = SendQuery("CURR?")
    return ret




if __name__ == "__main__":
    ret = SendQuery("CURR?")
    print(ret[0])





#https://github.com/MarkDing/GPIB-pyvisa
