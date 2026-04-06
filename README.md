# Tarea1
Programming III ; 2026-1 

## 3. Clase Tensor
Al inicio se creó la clase "Tensor.h" en el cual se definieron los constructores y destructores, así como una versión base de los métodos pero que luego se corrigió.

Luego se implementó el constructor y destructor respectivamente. Luego en el main se implementaron también los métodos de la clase Tensor (ones, zeros, random y arange).
En zeros se logró crear un vector del tamaño correcto rellenado con puros ceros. El mismo proceso se hizo con ones. Sin embargo, para random al inicio se planeó usar rand() pero no mantenía un control claro entre min y max, por lo cual se descartó parcialmente. 
La variable static se crea una sola vez en cada ejecución y se guarda si ya se inició el generador de números.
Se hizo un proceso donde se generaba con el rand(), se convertía el número a double, "RAND_MAX" es el número máximo posible. Se definió la variable r como la división entre rand() y RAND_MAX, asegurando que el número quede entre 0 y 1, posteriormente se escalaba al rango solicitado con una suma del mínimo.

Respecto al método arange, primero se definía cuantos números tendríamos, eso se logra con la resta entre end y start. Luego se crea un vector para "size" elementos y se llena el vector con valores consecutivos. Trabaja similar a python con np.arange

```c++
// Ejemplo 1
Tensor a = Tensor::arange(0, 5);
// size = 5-0 = 5
// values = [0, 1, 2, 3, 4]
// forma = {5}
// Resultado: [0, 1, 2, 3, 4]

// Ejemplo 2
Tensor b = Tensor::arange(2, 7);
// size = 7-2 = 5
// values = [2, 3, 4, 5, 6]
// Resultado: [2, 3, 4, 5, 6]

// Ejemplo 3
Tensor c = Tensor::arange(-3, 3);
// size = 3-(-3) = 6
// values = [-3, -2, -1, 0, 1, 2]
// Resultado: [-3, -2, -1, 0, 1, 2]

```

Una limitancia que se debe mencionar es que funciona con enteros, no podríamos incluir por ejemplo un Tensor::arange(0, 5.5)

## 4. Gestión de Memoria y Ciclo de Vida

Se implementaron los 5 métodos especiales:

- **Destructor (`~Tensor`)**: Libera la memoria con `delete[]` para evitar fugas.

- **Constructor de Copia**: Realiza deep copy, reservando nueva memoria y copiando todos los valores.

- **Asignador de Copia (`operator=`)**: Libera memoria actual, luego copia los nuevos datos. Incluye verificación de auto-asignación.

- **Constructor de Movimiento**: Transfiere la propiedad del puntero, dejando el origen en `nullptr`. Es `noexcept` para optimizaciones.

- **Asignador de Movimiento**: Similar al constructor de movimiento, pero aplicado a un objeto existente.

Estos métodos garantizan que no haya fugas de memoria, dobles liberaciones, y permiten operaciones eficientes sin copias innecesarias.

## 5. Polimorfismo y transformaciones

Se implementó un sistema polimórfico para aplicar transformaciones elemento a elemento a los tensores.

### Clases implementadas:

- **`TensorTransform`** (abstracta): Define la interfaz virtual `apply()`.
- **`ReLU`**: Implementa `y = max(0, x)`.
- **`Sigmoid`**: Implementa `y = 1/(1+e^(-x))`.

### Método `Tensor::apply()`:

Recibe una referencia a `TensorTransform` y devuelve un nuevo tensor con la transformación aplicada.

### Ejemplo:

```c++
ReLU relu;
Tensor B = A.apply(relu);
```

Eso aplica ReLU a cada elemento. Igualmente pondré un ejemplo de ejecución:

```c++
int main() {
    Tensor A = Tensor::arange(-5, 5);
    cout << "original: ";
    A.print();
    ReLU relu;
    Tensor B = A.apply(relu);
    cout << "despues de ReLU: ";
    B.print();
    Sigmoid sigmoid;
    Tensor C = A.apply(sigmoid);
    cout << "despues de Sigmoid: ";
    C.print();
    ReLU relu2;
    Tensor D = A.apply(relu2).apply(sigmoid);
    return 0;
}
```

Lo que se obtuvo en la ejecución:
```c++
original: [-5, -4, -3, -2, -1, 0, 1, 2, 3, 4]
despues de ReLU: [0, 0, 0, 0, 0, 0, 1, 2, 3, 4]
despues de Sigmoid: [0.00669285, 0.0179862, 0.0474259, 0.119203, 0.268941, 0.5, 0.731059, 0.880797, 0.952574, 0.982014]
```

Entonces podemos decir que agrega transformaciones sin tener que modificar 'Tensor' y el polimorfismo es en el tiempo de ejecución.

## 6. Sobrecarga de operadores

Se sobrecargaron los operadores `+`, `-` y `*` para trabajar con tensores de forma intuitiva.

Se utiliza el runtime_error porque cuando utilicé el cout convencional el programa seguía ejecutandose incluso cuando ambos vectores no coincidian y obviamente el programa daba resultados no deseados en la ejecución, por eso se utilizó el throw.


### Operadores implementados:

- **`+`** : Suma elemento a elemento
- **`-`** : Resta elemento a elemento
- **`*`** : Multiplicación elemento a elemento (Hadamard)
- **`*`** : Multiplicación por escalar (tensor × escalar y escalar × tensor)

### Ejemplo:

```c++
int main(){
    Tensor A = Tensor::arange(2, 5);
    Tensor B = Tensor::ones({3});
    Tensor C = A + B;
    Tensor D = A - B;
    Tensor E = A * B;
    Tensor F = A * 2.0;
    Tensor H = (A + B) * 2.0;
    A.print();
    B.print();
    C.print();
    D.print();
    E.print();
    F.print();
    H.print();
    return 0;
}
```
Resultado:
```c++
[2, 3, 4]
[1, 1, 1]
[3, 4, 5]
[1, 2, 3]
[2, 3, 4]
[4, 6, 8]
[6, 8, 10]
```

## 7. Modificación de dimensiones

Aquí se implementa dos métodos que permitan cambiar la forma de un tensor sin copiar los datos, esto lo hacemos con view() y unsqueeze().
Generamos la variable **`d_data`**, esta variable en realidad es "dueño_data", servirá para saber si el tensor es dueño de la memoria, hacemos esto porque si lo hacemos de forma tradicional haríamos un deep copy, pero no es lo que se busca.
También modificamos el destructor para que libere memoria si d_data es true, o sea, dueño de los datos.

El método **view()** permite reinterpretar la forma de un tensor sin modificar los datos subyacentes. Solo cambia la organización lógica de las dimensiones.

El método **unsqueeze()** agrega una dimensión de tamaño 1 en la posición especificada. Esto es útil para preparar tensores para operaciones matriciales.

Ejemplos de ejecución:
```c++
int main() {
    Tensor A = Tensor::arange(0, 12);
    A.print();
    Tensor B = A.view({3, 4});
    B.print();
    Tensor C = A.view({2, 2, 3});
    C.print();
    Tensor D = A.unsqueeze(0);
    Tensor E = B.unsqueeze(1);
    Tensor F = B.unsqueeze(2);
    return 0;
}
```
Resultado obtenido:
```c++
[0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11]
[0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11]
[0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11]
```

## 8. Concatenación
Se implementó el método estático `concat()` para unir múltiples tensores a lo largo de una dimensión específica. El método crea un nuevo tensor con memoria propia (deep copy) y copia los datos de cada tensor en la posición correspondiente dentro del nuevo arreglo contiguo.



### Método concat

```c++
static Tensor concat(const std::vector<Tensor>& tensors, size_t dim);
```

### Validaciones realizadas:

- Que haya al menos un tensor para concatenar
- Que la dimensión sea válida (dentro del rango)
- Que todos los tensores tengan el mismo número de dimensiones
- Que todas las dimensiones excepto la de concatenación coincidan
- Que el tensor resultante no exceda 3 dimensiones

### Ejemplos de ejecución:

```c++
Tensor A = Tensor::ones({2, 3});
Tensor B = Tensor::zeros({2, 3});
Tensor C = Tensor::concat({A, B}, 0);
Tensor D = Tensor::concat({A, B}, 1);
Tensor E = Tensor::concat({A, A, B, B}, 0);
C.print();
D.print();
E.print();
```

### Resultado de ejecución:

```cpp
[1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0]
[1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0]
[1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
```

## 9. Funciones amigas: dot y matmul

Se implementaron dos funciones amigas para operaciones algebraicas avanzadas, que pueden acceder directamente a la memoria interna de los tensores.

###  Producto punto (dot)

Calcula el producto punto entre dos vectores (tensores 1D).

```c++
friend Tensor dot(const Tensor& a, const Tensor& b);
```

## Validaciones realizadas
- Ambos tensores deben ser 1D
- Deben tener la misma longitud

### Multiplicación matricial (matmul)
Realiza la multiplicación de dos matrices (tensores 2D).

```c++
friend Tensor matmul(const Tensor& a, const Tensor& b);
```

## Validaciones:
- Ambos tensores deben ser 2D
- Deben tener la misma longitud

## Ejemplos de ejecución (ambos):
```c++
Tensor v = Tensor::arange(1, 5); // [1,2,3,4]
Tensor w = Tensor::arange(5, 9); // [5,6,7,8]
Tensor dp = dot(v, w);               // 1*5+2*6+3*7+4*8 = 70
dp.print();

    // Multiplicación matricial
    Tensor M1 = Tensor::random({3, 4}, -1, 1); // 3×4
    Tensor M2 = Tensor::random({4, 5}, -1, 1); // 4×5
    Tensor M3 = matmul(M1, M2); // 3×5
    M3.print();
```
Resultados:
```c++
[70]
[-0.0345116, -1.46236, 0.435877, -0.431674, 0.245255, -0.104995, -0.580882, -0.0983935, 0.30619, -0.284899, 0.446759, -0
.486285, -0.409464, -0.895415, 0.432224]
```
Estas funciones son importantes para la implementación de redes neuronales que haremos a continuación.

## 10. Redes Neuronales

Utilizando toda la funcionalidad de la librería Tensor++, se construyó una red neuronal de dos capas para procesamiento de datos.

###  Arquitectura de la red

| Capa | Operación | Dimensión de entrada | Dimensión de salida |
|------|-----------|---------------------|---------------------|
| Entrada | Datos  | - | 1000 × 20 × 20 |
| Flatten | `view()` | 1000 × 20 × 20 | 1000 × 400 |
| Capa lineal 1 | `matmul()` + bias | 1000 × 400 | 1000 × 100 |
| Activación 1 | `ReLU()` | 1000 × 100 | 1000 × 100 |
| Capa lineal 2 | `matmul()` + bias | 1000 × 100 | 1000 × 10 |
| Activación 2 | `Sigmoid()` | 1000 × 10 | 1000 × 10 |

###  Implementación

```c++
// 1. Entrada (1000 muestras de 20×20)
Tensor X = Tensor::random({1000, 20, 20}, 0.0, 1.0);

// 2. Aplanar
Tensor X_flat = X.view({1000, 400});

// 3. Primera capa lineal
Tensor W1 = Tensor::random({400, 100}, -0.1, 0.1);
Tensor b1 = Tensor::random({1, 100}, -0.1, 0.1);
Tensor Z1 = matmul(X_flat, W1);
Tensor A1 = Z1 + b1;

// 4. Activación ReLU
ReLU relu;
Tensor H1 = A1.apply(relu);

// 5. Segunda capa lineal
Tensor W2 = Tensor::random({100, 10}, -0.1, 0.1);
Tensor b2 = Tensor::random({1, 10}, -0.1, 0.1);
Tensor Z2 = matmul(H1, W2);
Tensor A2 = Z2 + b2;

// 6. Activación Sigmoid (salida final)
Sigmoid sigmoid;
Tensor Y = A2.apply(sigmoid);
```

### Red Neuronal implementada y su ejecución:

```c++
int main(){
cout << "     RED NEURONAL CON TENSOR++          " << endl;
try {
// ===== PASO 1: Entrada =====
cout << "\n[Paso 1] Creando tensor de entrada..." << endl;
Tensor X = Tensor::random({1000, 20, 20}, 0.0, 1.0);
print_shape(X, "X");

        // ===== PASO 2: View =====
        cout << "\n[Paso 2] Aplanando..." << endl;
        Tensor X_flat = X.view({1000, 400});
        print_shape(X_flat, "X_flat");

        // ===== PASO 3: Primera multiplicación =====
        cout << "\n[Paso 3] Primera capa lineal..." << endl;
        Tensor W1 = Tensor::random({400, 100}, -0.1, 0.1);
        print_shape(W1, "W1");

        Tensor Z1 = matmul(X_flat, W1);
        print_shape(Z1, "Z1 = X_flat × W1");

        // ===== PASO 4: Sumar bias =====
        cout << "\n[Paso 4] Sumando bias..." << endl;
        Tensor b1 = Tensor::random({1, 100}, -0.1, 0.1);
        print_shape(b1, "b1");

        Tensor A1 = Z1 + b1;
        print_shape(A1, "A1 = Z1 + b1");

        // ===== PASO 5: ReLU =====
        cout << "\n[Paso 5] Aplicando ReLU..." << endl;
        ReLU relu;
        Tensor H1 = A1.apply(relu);
        print_shape(H1, "H1 = ReLU(A1)");

        // ===== PASO 6: Segunda multiplicación =====
        cout << "\n[Paso 6] Segunda capa lineal..." << endl;
        Tensor W2 = Tensor::random({100, 10}, -0.1, 0.1);
        print_shape(W2, "W2");

        Tensor Z2 = matmul(H1, W2);
        print_shape(Z2, "Z2 = H1 × W2");

        // ===== PASO 7: Sumar bias =====
        cout << "\n[Paso 7] Sumando bias..." << endl;
        Tensor b2 = Tensor::random({1, 10}, -0.1, 0.1);
        print_shape(b2, "b2");

        Tensor A2 = Z2 + b2;
        print_shape(A2, "A2 = Z2 + b2");

        // ===== PASO 8: Sigmoid =====
        cout << "\n[Paso 8] Aplicando Sigmoid..." << endl;
        Sigmoid sigmoid;
        Tensor Y = A2.apply(sigmoid);
        print_shape(Y, "Y = Sigmoid(A2)");

        // ===== RESULTADO FINAL =====
        cout << "    ¡RED NEURONAL COMPLETADA CON EXITO!   " << endl;

        cout << "\nShape final del tensor de salida: [";
        for (size_t d : Y.getShape()) {
            cout << d << " ";
        }
        cout << "]" << endl;
        cout << "Total de parametros entrenables: "
                  << 400*100 + 100 + 100*10 + 10 << endl;
        cout << "  - W1: " << 400*100 << endl;
        cout << "  - b1: " << 100 << endl;
        cout << "  - W2: " << 100*10 << endl;
        cout << "  - b2: " << 10 << endl;

    } catch (const exception& e) {
        cerr << "\nError: " << e.what() << endl;
        return 1;
    }
    return 0;
}
```

## Resultados esperados:
```c++
Shape de entrada: [1000 20 20]
Shape después de view: [1000 400]
Shape después de primera capa: [1000 100]
Shape después de ReLU: [1000 100]
Shape después de segunda capa: [1000 10]
Shape de salida final: [1000 10]
```


## Resultados respectivos:
```c++
     RED NEURONAL CON TENSOR++

[Paso 1] Creando tensor de entrada...
X shape: [1000 20 20 ]

[Paso 2] Aplanando...
X_flat shape: [1000 400 ]

[Paso 3] Primera capa lineal...
W1 shape: [400 100 ]
Z1 = X_flat ├ù W1 shape: [1000 100 ]

[Paso 4] Sumando bias...
b1 shape: [1 100 ]
A1 = Z1 + b1 shape: [1000 100 ]

[Paso 5] Aplicando ReLU...
H1 = ReLU(A1) shape: [1000 100 ]

[Paso 6] Segunda capa lineal...
W2 shape: [100 10 ]
Z2 = H1 ├ù W2 shape: [1000 10 ]

[Paso 7] Sumando bias...
b2 shape: [1 10 ]
A2 = Z2 + b2 shape: [1000 10 ]

[Paso 8] Aplicando Sigmoid...
Y = Sigmoid(A2) shape: [1000 10 ]
┬íRED NEURONAL COMPLETADA CON EXITO!

Shape final del tensor de salida: [1000 10 ]
Total de parametros entrenables: 41110
- W1: 40000
- b1: 100
- W2: 1000
- b2: 10
```

Notamos que concuerda con lo planteado anteriormente, de esta manera se implementaría con éxito Tensor++.


