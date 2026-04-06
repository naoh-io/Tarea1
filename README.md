# Tarea1
Programming III ; 2026-1 

### 3. Clase Tensor
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

### 4. Gestión de Memoria y Ciclo de Vida

Se implementaron los 5 métodos especiales:

- **Destructor (`~Tensor`)**: Libera la memoria con `delete[]` para evitar fugas.

- **Constructor de Copia**: Realiza deep copy, reservando nueva memoria y copiando todos los valores.

- **Asignador de Copia (`operator=`)**: Libera memoria actual, luego copia los nuevos datos. Incluye verificación de auto-asignación.

- **Constructor de Movimiento**: Transfiere la propiedad del puntero, dejando el origen en `nullptr`. Es `noexcept` para optimizaciones.

- **Asignador de Movimiento**: Similar al constructor de movimiento, pero aplicado a un objeto existente.

Estos métodos garantizan que no haya fugas de memoria, dobles liberaciones, y permiten operaciones eficientes sin copias innecesarias.



