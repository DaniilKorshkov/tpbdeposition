import JSONoperators

def amperage_control(current_deposition_rate, target_deposit_rate, current_amperage):

    precision_interval = JSONoperators.ReadJSONConfig("deposition_parameters","precision_interval")


    proper_deposition_rate = True
    i = 0
    for element in current_deposition_rate:

        if abs( element - target_deposit_rate[i] ) > precision_interval:
            proper_deposition_rate = False
        i += 1

    if proper_deposition_rate:
        return current_amperage




    transformation_matrix = JSONoperators.ReadJSONConfig("deposition_parameters","transformation_matrix")


    new_amperage = [0,0,0,0]
    for i in range(4):
        amperage_difference_element = 0
        for j in range(4):
            amperage_difference_element += (  target_deposit_rate[j] - current_deposition_rate[j] )*( (transformation_matrix[i])[j]   )
        
        new_amperage[i] = current_amperage[i] + amperage_difference_element


    
    
    


    return new_amperage



if __name__ == "__main__":
    print(amperage_control([0,0,0,0], [1,1,1,0.5], [0,0,0,0]))