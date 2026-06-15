import keysight_power_supply as kp
import time
import datetime
import CreateNewFile


import JSONoperators as js
import signal
import sys


def main():



    #------------------------------ initiation --------------------------------------------

    js.MergeJSONConfigs()

    
    signal.signal(signal.SIGINT, kill_handler)
    signal.signal(signal.SIGTERM, kill_handler)

    filename = CreateNewFile.MakeNewFile()



    while True:
        try:
            interval = int(input(f"Enter record interval in seconds: "))
            assert interval > 0
            break
        except:
            print(f"Not a valid integer")




    

    print(f"To terminate the process, please use Ctrl+C")






    #------------------------------ infinite cycle --------------------------------------------


    while True:
        current_time = (datetime.datetime.now()).strftime("%Y-%h-%d %H:%M:%S")
        voltage = (kp.ReadVoltage("1"))[0]
        amperage = (kp.ReadAmperage("1"))[0]




        handle = open(filename, "a")
        handle.write(f"{current_time}\t\t")
        handle.write(f"{voltage}\t")
        handle.write("\t")
        handle.write(f"{amperage}\t")
        handle.write("\n")
        handle.close()



        time.sleep(interval)





def kill_handler(*args):
    sys.exit(0)


if __name__ == "__main__":
    main()
