import pyvisa
import JSONoperators as js
#import Logging
#import datetime

def SendCommand(command, config="MainConfig"):

    address = js.ReadJSONConfig("keysight_power_supply","address")

    rm = pyvisa.ResourceManager()
    keysight = rm.open_resource(address)

    
    keysight.write(command)
    keysight.close()

def SendQuery(command, config="MainConfig"):

    address = js.ReadJSONConfig("keysight_power_supply","address")

    rm = pyvisa.ResourceManager()
    keysight = rm.open_resource(address)

    
    ret = keysight.query_ascii_values(command)
    keysight.close()

    return ret



def ReadVoltage(channel):
    
    SendCommand(f"INSTR CH{channel}")
    ret = SendQuery("VOLT?")
    
    return ret


def ReadAmperage(channel):
    
    SendCommand(f"INSTR CH{channel}")
    ret = SendQuery("CURR?")
    
    return ret

def SetVoltage(channel,voltage):
    
    SendCommand(f"INSTR CH{channel}")
    ret = SendCommand(f"VOLT {voltage}")
    
    return ret


def SetAmperage(channel,amperage):
    
    SendCommand(f"INSTR CH{channel}")
    ret = SendCommand(f"CURR {amperage}")
    
    return ret





if __name__ == "__main__":
    ret = SendQuery("CURR?")
    print(ret[0])





#https://github.com/MarkDing/GPIB-pyvisa
