Import("env")
print("Replace MKSPIFFSTOOL with mklittlefs")
# replace with .exe file for windows if required
env.Replace (MKSPIFFSTOOL = "./mklittlefs")
