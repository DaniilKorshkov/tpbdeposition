import cygnus_config_processor as ccp
import JSONoperators as js
from keysight_power_supply import SetAmperage
import time
import numpy as np


def main():
    #{"class": "cygnus2", "log_filename": "test1", "power_supply_output_index": 0} 


    js.MergeJSONConfigs()
    

    log_filename = js.ReadJSONConfig("cygnus2","log_filename")
    power_supply_output_index = js.ReadJSONConfig("cygnus2","power_supply_output_index") # specifies which log entry has Output value
    deposition_rate_index = js.ReadJSONConfig("cygnus2","deposition_rate_index") # specifies which log entry has Output value
    sleep_time = js.ReadJSONConfig("keysight_power_supply","sleep_time")
    maximal_amperage = js.ReadJSONConfig("keysight_power_supply","maximal_amperage")


    initial_time = datetime.datetime.now().timestamp()
    last_recording_time = datetime.datetime.now() 

    while True: # as r1 stability determined at user discretion, program is started. Average r1 recorded over 5 seconds




        if ( datetime.datetime.now() -  last_recording_time) > 1:

            r1_array = []
            current_deposition_rate = ccp.get_entry(log_filename, deposition_rate_index)
            r1_array.append(current_deposition_rate)

            if len(r1_array) >= 5:
                r1 = float(numpy.mean(r1_array))
                sigma_r = float(numpy.std(r1_array))
                break
            

            last_recording_time = datetime.datetime.now()

        
        
    
    
    
    SetAmperage(1, 5.1)  # Apply upward current step
    initial_time = datetime.datetime.now().timestamp()

    L_already_triggered = False
    r2_already_triggered = False 


    while True: # Continue recording r_filtered while
        if ( datetime.datetime.now() -  last_recording_time) > 1:



            r_array = []
            current_deposition_rate = ccp.get_entry(log_filename, deposition_rate_index)
            r_array.append(current_deposition_rate)

            time.sleep(1)

            if ((current_deposition_rate - r1) > (3*sigma_r)) and not L_already_triggered:  
                L = (datetime.datetime.now().timestamp()) - initial_time
                L_already_triggered = True

        

            if L_already_triggered and (len(r_array) > 10):
                array_1 = (r_array[-5:-1])
                array_1.append(r_array[-1])

                array_2 = (r_array[-11:-6])



                if (abs( float(numpy.mean(array_1)) - float(numpy.mean(array_2))  ) < (2*float(numpy.std(array_1)))) and not r2_already_triggered:
                    r2 = float(numpy.mean(array_1))
                    r2_already_triggered = True
                    break


            last_recording_time = datetime.datetime.now()


    P1 = 25.00
    P2 = 26.01

    K_process = (r2 - r1) / (P2 - P1)
    r_632 = r1 + 0.632*( r2 - r1 )

    for element in r_array:
        i = 0
        if element >= r_632:
            t_632 = i
            break


    T1 = r_632 + L

    Kc = (1.36 / K_process) * (L / T1)^(-0.947)

    Ti = 1.19*T1 * (L / T1)^(0.738)

    Td = 0.381*T1 * (L / T1)^(0.995)

    


        #power_percentage = ccp.get_entry(log_filename, power_supply_output_index)


        amperage = (power_percentage**0.5)*maximal_amperage
        SetAmperage(1, amperage)

        print(f"Power %: {power_percentage}, amperage: {amperage}")
        time.sleep(sleep_time)








if __name__ == "__main__":
    main()