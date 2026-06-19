from collections import deque





def get_power_percentage(log_filename, power_output_index):

    #{"class": "cygnus2", "config_filename": "test1", "power_supply_output_index": 0} 

    


    with open(log_filename, 'r') as f:
        last_line = deque(f, maxlen=1).pop().strip()
        if not("Cygnus2" in last_line):
            power_percentage = float(( last_line.split(" ") )[power_output_index])

    return power_percentage


if __name__ == "__main__":
    print(get_power_percentage("test.txt", 0))