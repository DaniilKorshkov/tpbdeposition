import subprocess
import JSONoperators as js
import sys




def WriteFirstLineInNewFile(path):
    with open(filename,"a") as handle:
        handle.write(f"Current time \t\t")
        
        for sensor_number in js.ReadJSONConfig("keysight_power_supply","currently_processed_channels"):
            handle.write(f"Port {sensor_number} voltage\t")
            handle.write(f"Port {sensor_number} amperage\t")
            
        
        handle.write(f"\n\n")
    
    


def MakeNewFile():
    
    operating_system = sys.platform
    current_directory = js.ReadJSONConfig("Technical","current_directory")


    if operating_system == 'linux':

        while True:
            filename = str(input("Enter filename: "))
            type = str(subprocess.run([f"file", f"{filename}"], cwd=current_directory, capture_output=True).stdout)
            type = type[(len(filename)+4):(len(type)-3)]


            if (type == f"cannot open `{filename}' (No such file or directory)" or type == "empty" or type == "ASCII text") and filename != "MainConfig":
                break
            else:
                print("Invalid filename")
            
        subprocess.run([f"touch", f"{filename}"], cwd=current_directory)
        if type == f"cannot open `{filename}' (No such file or directory)":
            
            WriteFirstLineInNewFile(filename)
            

        return filename

    
    else:

       
   


        #For windows specify directory: r"C:\Users\COLDlab6\Desktop\tpbdeposition"
        while True:
            filename = input("Enter filename (e.g., data.txt): ").strip()
            
            if filename == "MainConfig" or not filename:
                print("Invalid filename.")
            else:
                break
                
            # Combine directory and filename into an absolute path
        full_path = os.path.join(current_directory, filename)
        
        # Check if the file already exists
        if os.path.exists(full_path):
            pass
        else:
            # File does not exist, which is what we want
            WriteFirstLineInNewFile(full_path)

        
        return full_path

