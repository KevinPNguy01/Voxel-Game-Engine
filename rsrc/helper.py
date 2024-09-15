import os

os.chdir("temp")

for file in os.listdir("."):
	name = file.replace(".png", "")
	with open(f"{name}.txt", "w") as f:
		for i in range(6):
			f.write(f"{name}\n")