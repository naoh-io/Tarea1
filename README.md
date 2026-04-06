# Tarea1
Programming III ; 2026-1 

## 3. Clase Tensor
Al inicio se creó la clase "Tensor.h" en el cual se definieron los constructores y destructores, así como una versión base de los métodos pero que luego se corrigió.

Luego se implementó el constructor y destructor respectivamente. Luego en el main se implementaron también los métodos de la clase Tensor (ones, zeros, random y arange).
En zeros se logró crear un vector del tamaño correcto rellenado con puros ceros. El mismo proceso se hizo con ones. Sin embargo, para random al inicio se planeó usar rand() pero no mantenía un control claro entre min y max, por lo cual se descartó parcialmente. 
La variable static se crea una sola vez en cada ejecución y se guarda si ya se inició el generador de números.
Se hizo un proceso donde se generaba con el rand(), se convertía el número a double, "RAND_MAX" es el número máximo posible. Se definió la variable r como la división entre rand() y RAND_MAX, asegurando que el número quede entre 0 y 1, posteriormente se escalaba al rango solicitado con una suma del mínimo.

Respecto al método arange, primero se definía cuantos números tendríamos, eso se logra con la resta entre end y start. Luego se crea un vector para "size" elementos y se llena el vector con valores consecutivos. Trabaja similar a python con np.arange

```{c++}
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

```cpp
ReLU relu;
Tensor B = A.apply(relu);
```

Eso aplica ReLU a cada elemento. Igualmente pondré un ejemplo de ejecución:

```cpp
int main() {
    Tensor A = Tensor::arange(-5, 5);
    std::cout << "original: ";
    A.print();
    ReLU relu;
    Tensor B = A.apply(relu);
    std::cout << "despues de ReLU: ";
    B.print();
    Sigmoid sigmoid;
    Tensor C = A.apply(sigmoid);
    std::cout << "despues de Sigmoid: ";
    C.print();
    ReLU relu2;
    Tensor D = A.apply(relu2).apply(sigmoid);
    return 0;
}
```

Lo que se obtuvo en la ejecución:
```cpp
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

```cpp
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
```cpp
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
```cpp
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
```cpp
[0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11]
[0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11]
[0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11]
```

## 8. Concatenación
Se implementó el método estático `concat()` para unir múltiples tensores a lo largo de una dimensión específica. El método crea un nuevo tensor con memoria propia (deep copy) y copia los datos de cada tensor en la posición correspondiente dentro del nuevo arreglo contiguo.



### Método concat

```cpp
static Tensor concat(const std::vector<Tensor>& tensors, size_t dim);
```

### Validaciones realizadas:

- Que haya al menos un tensor para concatenar
- Que la dimensión sea válida (dentro del rango)
- Que todos los tensores tengan el mismo número de dimensiones
- Que todas las dimensiones excepto la de concatenación coincidan
- Que el tensor resultante no exceda 3 dimensiones

### Ejemplos de ejecución:

```cpp
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

