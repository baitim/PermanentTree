import random
import os

count_tests = 5

to_curr_dir = os.path.dirname(os.path.realpath(__file__))

for test_num in range(0, count_tests) :
    file_name = to_curr_dir + "/tests_in/test_" + f'{test_num+1:03}' + ".in"
    file = open(file_name, 'w')

    count_commands = 1000
    reset_chance = 0.25
    keys = [0, 10000]

    for i in range(count_commands):
        command = random.randint(0, 1)
        test_str = ""

        if (command == 1):
            test_str = "s "

        if (command <= 1):
            test_str += "k " + str(random.randint(keys[0], keys[1])) + "\n"

        if (command == 1):
            reset = random.randint(0, 100)
            if (reset < reset_chance * 100):
                test_str += "r\n"

        file.write(test_str)
    
    file.close()
    print("test ", test_num + 1, " generated")