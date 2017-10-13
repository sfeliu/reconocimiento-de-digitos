import numpy as np
import matplotlib.pyplot as plt
import pickle, sys


def get_accuracy(matrix, clase):
    true_positive = matrix[clase, clase]
    fila = matrix[clase, :]
    columna = matrix[:, clase]
    fila_sum = fila.sum() - true_positive
    false_negative = fila_sum
    col_sum = columna.sum()
    false_positive = col_sum - true_positive
    matrix_sum = matrix.sum()
    true_negative = (matrix_sum - fila_sum) - col_sum
    accuracy = (true_negative + true_positive) / (true_negative + true_positive + false_negative + false_positive)
    return accuracy


def get_precision(matrix, clase):
    true_positive = matrix[clase, clase]
    columna = matrix[:, clase]
    false_positive = columna.sum() - true_positive
    precision = true_positive / (true_positive + false_positive)
    return precision


def get_recall(matrix, clase):
    true_positive = matrix[clase, clase]
    fila = matrix[clase, :]
    false_negative = fila.sum() - true_positive
    precision = true_positive / (true_positive + false_negative)
    return precision


def get_F1_score(precision, recall):
    return 2 * ((precision * recall) / (precision + recall))


def levantar_info(filename, parametros):
    file = 'data/Resultados/' + filename
    contenido = []
    for line in open(file):
        contenido.append(line.strip())

    contenido = contenido[1:]
    inferencias_por_clase = []

    for i in range(0, 10):
        inferencia = []
        for j in range(0, 10):
            inferencia.append(0)
        inferencias_por_clase.append(inferencia)

    contador = 0
    autovalores_autovectores = {}
    for linea in contenido:
        linea = linea.split(',')
        clase = int(linea[1])
        inferencia = int(linea[2])
        inferencias_por_clase[clase][inferencia] += 1

    for clase, lista in enumerate(inferencias_por_clase):
        for index, inferencia in enumerate(inferencias_por_clase[clase]):
            inferencias_por_clase[clase][index] = round(inferencia / int(parametros['K']))

    return inferencias_por_clase


# def crear_AUC(datos, filename):





def crear_matriz_de_confusion(inferencias_por_clase, parameters):
    norm_conf = []
    for clase in inferencias_por_clase:
        tmp_arr = []
        a = sum(clase, 0)
        for inferencia in clase:
            tmp_arr.append(float(inferencia) / float(a))
        norm_conf.append(tmp_arr)

    fig = plt.figure()
    plt.clf()
    ax = fig.add_subplot(111)
    ax.set_aspect(1)
    res = ax.imshow(np.array(norm_conf), cmap=plt.cm.Wistia,
                    interpolation='nearest')

    width, height = inferencias_por_clase.shape

    for x in range(width):
        for y in range(height):
            ax.annotate(str(inferencias_por_clase[x][y]), xy=(y, x),
                        horizontalalignment='center',
                        verticalalignment='center')

    cb = fig.colorbar(res)
    clases = '0123456789'
    plt.xticks(range(width), clases[:width])
    plt.yticks(range(height), clases[:height])
    plt.xlabel('Predicciones', fontsize=18)
    plt.ylabel('Datos', fontsize=16)
    plt.suptitle(
        'Matriz de confución con alpha=' + parameters['alpha'] + ' k=' + parameters['k'] + ' K=' + parameters['K'],
        fontsize=15)
    matrix_file = 'alpha_' + parameters['alpha'] + '_k_' + parameters['k'] + '_K_' + parameters['K'] + '.png'
    plt.savefig(matrix_file, format='png')


def main(filename, crear_matriz):
    parametros = {
        'alpha': '3',
        'k': '4',
        'K': '4'
    }
    inferencias_por_clase = levantar_info(filename, parametros)

    inferencias_por_clase = np.array(inferencias_por_clase)
    if crear_matriz:
        crear_matriz_de_confusion(inferencias_por_clase, parametros)

    data_por_clase = {}
    for clase in range(0, 10):
        data_por_clase[clase] = {}
        data_por_clase[clase]['accuracy'] = get_accuracy(inferencias_por_clase, clase)
        data_por_clase[clase]['precision'] = get_precision(inferencias_por_clase, clase)
        data_por_clase[clase]['recall'] = get_recall(inferencias_por_clase, clase)
        data_por_clase[clase]['F1_score'] = get_F1_score(data_por_clase[clase]['precision'],
                                                         data_por_clase[clase]['recall'])

    with open('data.pickle', 'rb') as handle:
        data_por_experimento = pickle.load(handle)

    data_por_experimento[filename] = data_por_clase
    # data_por_experimento = {}

    with open('data.pickle', 'wb') as handle:
        pickle.dump(data_por_experimento, handle)


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print('Correr de la siguiente manera: python creadorImagen.py <filename> <guardarImagen=True-False>')
    else:
        filename = sys.argv[1]
        if len(sys.argv) < 3:
            print('Correr de la siguiente manera: python creadorImagen.py <filename> <guardarImagen=True-False>')
        if sys.argv[2] == 'True':
            crear_matriz = True
        else:
            crear_matriz = False

        main(filename, crear_matriz)
