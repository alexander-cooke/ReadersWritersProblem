import random

task_file = open("task_file_100_with_5.txt", "w")
x = 0

for i in range(100):
    line = str(x) + " " + str(random.randint(1,5)) + "\n"
    task_file.write(line)
    x = x + 1
    
task_file.close()

