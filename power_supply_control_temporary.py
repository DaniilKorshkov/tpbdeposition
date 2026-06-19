import cygnus_config_processor as ccp
import JSONoperators as js
from keysight_power_supply import SetAmperage
import time


def main():
    #{"class": "cygnus2", "log_filename": "test1", "power_supply_output_index": 0} 


    js.MergeJSONConfigs()
    

    log_filename = js.ReadJSONConfig("cygnus2","log_filename")
    power_supply_output_index = js.ReadJSONConfig("cygnus2","power_supply_output_index")
    maximal_amperage = js.ReadJSONConfig("keysight_power_supply","maximal_amperage")


    while True:
        power_percentage = ccp.get_power_percentage(log_filename, power_supply_output_index)
        amperage = (power_percentage**0.5)*maximal_amperage
        #SetAmperage(1, amperage)

        print(f"Power %: {power_percentage}, amperage: {amperage}")
        time.sleep(0.1)








if __name__ == "__main__":
    main()