import numpy as np
import matplotlib.pyplot as plt
import pickle, sys, re

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
    file = 'cv_K_distinto/' + filename
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


def levantar_diccionario():
    data_total = {}
    for line in open('metricas_K.csv', 'r'):
        if 'Fold' not in line:
            info_temporal = line.strip().split(',')
            number_f = info_temporal[0]
            number_a = info_temporal[1]
            number_k = info_temporal[2]
            clase = info_temporal[3]
            accuracy = info_temporal[4]
            precision = info_temporal[5]
            recall = info_temporal[6]
            f1_score = info_temporal[7]
            if number_f not in data_total.keys():
                data_total[number_f] = {}
            if number_a not in data_total[number_f].keys():
                data_total[number_f][number_a] = {}
            if number_k not in data_total[number_f][number_a].keys():
                data_total[number_f][number_a][number_k] = {}
            if clase not in data_total[number_f][number_a][number_k].keys():
                data_total[number_f][number_a][number_k][str(clase)] = {}
            data_total[number_f][number_a][number_k][str(clase)]['accuracy'] = accuracy
            data_total[number_f][number_a][number_k][str(clase)]['precision'] = precision
            data_total[number_f][number_a][number_k][str(clase)]['recall'] = recall
            data_total[number_f][number_a][number_k][str(clase)]['F1_score'] = f1_score
    return data_total

def guardar_diccionario(data):
    with open('metricas_K.csv', 'w') as f:
        f.write('Fold,Alpha,kNN,Clase,Accuracy,Presicion,Recall,F1_score\n')
        for fold in sorted(data.keys()):
            for alpha in sorted(data[fold].keys()):
                for knn in sorted(data[fold][alpha].keys()):
                    for clase in sorted(data[fold][alpha][knn].keys()):
                        acuracy = data[fold][alpha][knn][clase]['accuracy']
                        precision = data[fold][alpha][knn][clase]['precision']
                        recall = data[fold][alpha][knn][clase]['recall']
                        f1_score = data[fold][alpha][knn][clase]['F1_score']
                        f.write(fold + ',' + alpha + ',' + knn + ',' + clase + ',' + str(acuracy) + ',' + str(precision) + ',' + str(recall) + ',' + str(f1_score) + '\n')



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
    res = ax.imshow(np.array(norm_conf), interpolation='nearest')

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
        'Matriz de confucion con number_a=' + parameters['number_a'] + ' k=' + parameters['k'] + ' K=' + parameters['K'],
        fontsize=15)
    matrix_file = 'number_a_' + parameters['number_a'] + '_k_' + parameters['k'] + '_K_' + parameters['K'] + '.png'
    plt.savefig(matrix_file, format='png')


def main(filename, crear_matriz):
    number_k = re.findall('k([0-9]+)', filename)
    number_k =  number_k[0]
    number_f = re.findall('f([0-9]+)', filename)
    number_f = number_f[0]
    number_a = re.findall('a([0-9]+)', filename)
    number_a = number_a[0]
    parametros = {
        'number_a': number_a,
        'k': number_k,
        'K': number_f
    }
    inferencias_por_clase = levantar_info(filename, parametros)

    inferencias_por_clase = np.array(inferencias_por_clase)
    if crear_matriz:
        crear_matriz_de_confusion(inferencias_por_clase, parametros)

    temp_acuracy = 0
    temp_precision = 0
    temp_recall = 0
    temp_f1 = 0

    data_total = levantar_diccionario()

    if number_f not in data_total.keys():
        data_total[number_f] = {}
    if number_a not in data_total[number_f].keys():
        data_total[number_f][number_a] = {}
    if number_k not in data_total[number_f][number_a].keys():
        data_total[number_f][number_a][number_k] = {}
    for clase in range(0, 10):
        data_total[number_f][number_a][number_k][str(clase)] = {}
        data_total[number_f][number_a][number_k][str(clase)]['accuracy'] = get_accuracy(inferencias_por_clase, clase)
        temp_acuracy += data_total[number_f][number_a][number_k][str(clase)]['accuracy']
        data_total[number_f][number_a][number_k][str(clase)]['precision'] = get_precision(inferencias_por_clase, clase)
        temp_precision += data_total[number_f][number_a][number_k][str(clase)]['precision']
        data_total[number_f][number_a][number_k][str(clase)]['recall'] = get_recall(inferencias_por_clase, clase)
        temp_recall += data_total[number_f][number_a][number_k][str(clase)]['recall']
        data_total[number_f][number_a][number_k][str(clase)]['F1_score'] = get_F1_score(data_total[number_f][number_a][number_k][str(clase)]['precision'],
                                                         data_total[number_f][number_a][number_k][str(clase)]['recall'])
        temp_f1 += data_total[number_f][number_a][number_k][str(clase)]['F1_score']


    total_acuracy = temp_acuracy/10
    total_precision = temp_precision/10
    total_recall = temp_recall/10
    total_f1 = temp_f1/10

    data_total[number_f][number_a][number_k]['average'] = {}
    data_total[number_f][number_a][number_k]['average']['accuracy'] = total_acuracy
    data_total[number_f][number_a][number_k]['average']['precision'] = total_precision
    data_total[number_f][number_a][number_k]['average']['recall'] = total_recall
    data_total[number_f][number_a][number_k]['average']['F1_score'] = total_f1

    print('Accuracy:' + str(total_acuracy))
    print('Precision:' + str(total_precision))
    print('Recall:' + str(total_recall))
    print('F1 Score:' + str(total_f1))

    guardar_diccionario(data_total)



   # with open('data.pickle', 'rb') as handle:
    #    data_por_experimento = pickle.load(handle)

   # data_por_experimento[filename] = data_total[number_f][number_a][number_k]
    # data_por_experimento = {}

    #with open('data.pickle', 'wb') as handle:
     #   pickle.dump(data_por_experimento, handle)


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