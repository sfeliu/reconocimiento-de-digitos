import subprocess, os, re


for file in os.listdir('cv_K_distinto/'):
    test = subprocess.Popen('python creadorImagen.py ' + file + ' False', stdout=subprocess.PIPE, shell=True)
    stdout = test.communicate()[0].decode()
    print(stdout)
    sasa = 2