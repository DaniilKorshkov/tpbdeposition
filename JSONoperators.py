import json
import os
import time
import datetime


def ReadJSONConfig(linename,entryname,config="MainConfig"): #function to read a specific entry from specified line in config
    handle = open(config, "r")
    for line in handle:

        if line == "" or line == "\n" or line[0] == "#" or line == None:
            continue

        dict_line = json.loads(line)
        if dict_line["class"] == linename:
            entry = dict_line[entryname]
            break
    handle.close()

    if entry == None:
        raise LookupError(f"{entryname} entry was not found in {linename} line in {config} config")

    return entry






def MergeJSONConfigs(MainConfig="MainConfig",DefaultMainConfig="DefaultMainConfig"):
    MergedConfig = dict()
    LinesList = []

    #print(123)

    try:
        handle = open(MainConfig,"r")
        MainConfigExist = True
    except:
        MainConfigExist = False

    if MainConfigExist:

        handle = open(MainConfig,"r")
        for line in handle:
            dictline = json.loads(line)
            MergedConfig[dictline["class"]] = dictline
            LinesList.append(dictline["class"])
        handle.close()


        handle = open(DefaultMainConfig,"r")
        for line in handle:
            dictline = json.loads(line)
            if not (dictline["class"] in LinesList):
                MergedConfig[dictline["class"]] = dictline
            else:
                for key in MergedConfig:
                    if (MergedConfig[key])["class"] == dictline["class"]:
                        for entry_key in dictline:
                            try:
                                void = (MergedConfig[key])[entry_key]
                            except:
                                (MergedConfig[key])[entry_key] = dictline[entry_key]


        handle.close()



        handle = open(MainConfig,"w")
        for key in MergedConfig:
            handle.write(  json.dumps(MergedConfig[key])  )
            handle.write("\n")
        handle.close()

    else:
        NewConfig = []
        handle = open(DefaultMainConfig, "r")
        for line in handle:
            NewConfig.append(line)
        handle.close()

        handle = open(MainConfig, "w")
        for line in NewConfig:
            handle.write(line)
        handle.close()







def ReadCSV(filename, entries_to_display):
    handle = open(filename,"r")


    temperature_sensors_list = []
    pressure_sensors_list = []


    time_array = []
    temperature_arrays = {}
    pressure_arrays = {}


    temperature_sensors_count = 0 
    pressure_sensors_count = 0



    for line in handle:
        if "Current time" in line:    #handle first line

            splitline = line.split('\t')
            

            for element in splitline:
                if "Temp." in element:
                    temperature_sensors_list.append(element)
                    temperature_arrays[element] = []
                if "Press." in element:
                    pressure_sensors_list.append(element)
                    pressure_arrays[element] = []
            

        
        elif line == "" or line == "\n":
            pass
            



        else:  #handle other lines

    

            splitline = line.split('\t')

            
            utc_time = (datetime.datetime.fromtimestamp(time.mktime(time.strptime(splitline[0],"%Y-%b-%d %H:%M:%S")))).timestamp()
    

            time_array.append(utc_time)
            if len(time_array) > entries_to_display:
                    time_array.pop(0)

            
            i = 1

            for sensor in temperature_sensors_list:

                while splitline[i] == "" or splitline[i] == "\n" or splitline[i] == "\t":
                    i += 1


                temperature_arrays[sensor].append(float(splitline[i]))
                i += 1


                if len(temperature_arrays[sensor]) > entries_to_display:
                    temperature_arrays[sensor].pop(0)


            for sensor in pressure_sensors_list:

                
                while splitline[i] == "" or splitline[i] == "\n" or splitline[i] == "\t":
                    i += 1



                pressure_arrays[sensor].append(float(splitline[i]))
                i += 1

                if len(pressure_arrays[sensor]) > entries_to_display:
                    pressure_arrays[sensor].pop(0)

    handle.close()


    return time_array, temperature_arrays, pressure_arrays


if __name__ == "__main__":
        print(123)
        print(ReadCSV("b",100))


    
