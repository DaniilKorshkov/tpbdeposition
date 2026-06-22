import cygnus_config_processor as ccp
import JSONoperators as js
from keysight_power_supply import SetAmperage
import time


def main():
    #{"class": "cygnus2", "log_filename": "test1", "power_supply_output_index": 0} 


    js.MergeJSONConfigs()
    

    log_filename = js.ReadJSONConfig("cygnus2","log_filename")
    power_supply_output_index = js.ReadJSONConfig("cygnus2","power_supply_output_index") # specifies which log entry has Output value
    sleep_time = js.ReadJSONConfig("keysight_power_supply","sleep_time")
    maximal_amperage = js.ReadJSONConfig("keysight_power_supply","maximal_amperage")


    while True:
        power_percentage = ccp.get_power_percentage(log_filename, power_supply_output_index)
        amperage = (power_percentage**0.5)*maximal_amperage
        SetAmperage(1, amperage)

        print(f"Power %: {power_percentage}, amperage: {amperage}")
        time.sleep(sleep_time)








if __name__ == "__main__":
    main()