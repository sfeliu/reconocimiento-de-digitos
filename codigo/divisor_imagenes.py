import numpy as np
import matplotlib.pyplot as plt
import pickle, sys, re


def levantar_imagenes():
    file = 'data/train.csv'
    contenido = []
    for line in open(file):
        contenido.append(line.strip())

    intro = contenido[0]
    contenido = set(contenido[1:])
    imagenes_por_clase = {}

    contador = 0
    for linea in contenido:
        linea = linea.split(',', 1)
        clase = linea[0]
        imagen = linea[1]
        if clase in imagenes_por_clase.keys():
            imagenes_por_clase[clase].append(imagen)
        else:
            imagenes_por_clase[clase] = [imagen]

    return (intro,imagenes_por_clase)


def guardar_imagenes(imagenes, intro):

    with open('data/train_' + str(cant*10) + '.csv', 'w') as f:
        f.write(intro + '\n')
        for clase, imagen in imagenes:
                f.write(clase + ',' + imagen + '\n')

def main(cant):
    intro_mas_dic = levantar_imagenes()
    intro = intro_mas_dic[0]
    imagenes_por_clase = intro_mas_dic[1]
    imagenes = set()
    for clase in imagenes_por_clase:
        for i in range(0, cant):
            imagenes.add((clase,imagenes_por_clase[clase].pop()))
    guardar_imagenes(imagenes, intro)


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print('Correr de la siguiente manera: python divisor_imagenes.py <cantidad_por_clase>')
    else:
        cant = int(sys.argv[1])
        main(cant)