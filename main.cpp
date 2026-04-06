#include <iostream>
#include "Tensor.h"
#include <cmath>
using namespace std;

//Polimorfismo---------------

class TensorTransform {
public:
    virtual Tensor apply(const Tensor& t) const = 0;
    virtual ~TensorTransform() = default;
};

class ReLU : public TensorTransform {
public:
    Tensor apply(const Tensor& t) const override;
};

Tensor ReLU::apply(const Tensor& t) const {
    vector<double> values(t.getTotalSize());

    for (size_t i = 0; i < t.getTotalSize(); i++) {
        double x = t.getData()[i];
        values[i] = (x > 0) ? x : 0.0;  // Si x > 0, queda x; sino, 0
    }

    return Tensor(t.getShape(), values);
}

Tensor Tensor::apply(const TensorTransform& transform) const {
    return transform.apply(*this);
}

class Sigmoid : public TensorTransform {
public:
    Tensor apply(const Tensor& t) const override;
};

Tensor Sigmoid::apply(const Tensor& t) const {
    vector<double> values(t.getTotalSize());

    for (size_t i = 0; i < t.getTotalSize(); i++) {
        double x = t.getData()[i];
        values[i] = 1.0 / (1.0 + exp(-x));
    }

    return Tensor(t.getShape(), values);
}

//Metodos------------------------------

Tensor::Tensor(const vector<size_t>& shape, const vector<double>&values) : shape(shape) {
    total_size = 1;
    for (size_t dim : shape) {
        total_size *= dim;
    }

    if (values.size() != total_size) {
        cout << "La cantidad de valores no coincide con la forma del tensor" << endl;
    }

    data = new double[total_size];

    for (size_t i = 0; i < total_size; i++) {
        data[i] = values[i];
    }
}

Tensor::~Tensor() {
    if (d_data) {
        delete[] data;
    }
}

//copia
Tensor::Tensor(const Tensor& other)
    : shape(other.shape), total_size(other.total_size), d_data(true) {
    data = new double[total_size];
    for (size_t i = 0; i < total_size; i++) {
        data[i] = other.data[i];
    }
}

//asignador copia
Tensor& Tensor::operator=(const Tensor& other) {
    if (this == &other) return *this;

    delete[] data;
    shape = other.shape;
    total_size = other.total_size;
    data = new double[total_size];
    for (size_t i = 0; i < total_size; i++) {
        data[i] = other.data[i];
    }
    return *this;
}

//c. mov
Tensor::Tensor(Tensor&& other) noexcept
    : data(other.data),
      shape(move(other.shape)),
      total_size(other.total_size), d_data(other.d_data) {
    other.data = nullptr;
    other.total_size = 0;
    other.d_data = false;
}

//a. mov
Tensor& Tensor::operator=(Tensor&& other) noexcept {
    if (this == &other) return *this;

    delete[] data;
    data = other.data;
    shape = move(other.shape);
    total_size = other.total_size;

    other.data = nullptr;
    other.total_size = 0;

    return *this;
}

Tensor Tensor::zeros(const vector<size_t>& shape) {
    size_t total_size = 1;
    for (size_t dim : shape) {
        total_size *= dim;
    }
    vector<double> values(total_size, 0.0);
    return Tensor(shape, values);
}

Tensor Tensor::ones(const vector<size_t>& shape) {
    size_t total_size = 1;
    for (size_t dim : shape) {
        total_size *= dim;
    }

    vector<double> values(total_size, 1.0);
    return Tensor(shape, values);
}

Tensor Tensor::random(const vector<size_t>& shape, double min, double max) {
    static bool seeded = false;
    if (!seeded) {
        srand(time(nullptr));
        seeded = true;
    }

    size_t total_size = 1;
    for (size_t dim : shape) {
        total_size *= dim;
    }

    vector<double> values(total_size);
    for (size_t i = 0; i < total_size; i++) {
        double r = (double)rand()/RAND_MAX;
        values[i] = min+r*(max-min);
    }
    return Tensor(shape, values);
}

Tensor Tensor::arange(double start, double end) {
    size_t size = static_cast<size_t> (end-start);
    vector<double> values(size);
    for (size_t i = 0; i < size; i++) {
        values[i] = start + i;
    }
    return Tensor({size}, values);
}

//funcion para imprimir ----------------

void Tensor::print() const {
    cout << "[";
    for (size_t i = 0; i < total_size; i++) {
        cout << data[i];
        if (i < total_size - 1) cout << ", ";
    }
    cout << "]" << endl;
}

//Sobrecarga--------------------
Tensor Tensor::operator+(const Tensor& other) const {
    if (shape == other.shape) {
        vector<double> resultado_values(total_size);
        for (size_t i = 0; i < total_size; i++) {
            resultado_values[i] = data[i] + other.data[i];
        }
        return Tensor(shape, resultado_values);
    }

    if (shape.size() == 2 && other.shape.size() == 2 &&
        other.shape[0] == 1 && shape[1] == other.shape[1]) {

        vector<double> resultado_values(total_size);
        for (size_t i = 0; i < shape[0]; i++) {
            for (size_t j = 0; j < shape[1]; j++) {
                resultado_values[i * shape[1] + j] = data[i * shape[1] + j] + other.data[j];
            }
        }
        return Tensor(shape, resultado_values);
        }

    throw std::runtime_error("Dimensiones incompatibles para suma");
}

Tensor Tensor::operator-(const Tensor& other) const {
    if (shape != other.shape) {
        throw runtime_error("las dimensiones son incompatibles para realizar la resta");
    }
    vector<double> result_values(total_size);
    for (size_t i = 0; i < total_size; i++) {
        result_values[i] = data[i] - other.data[i];
    }
    return Tensor(shape, result_values);
}

Tensor Tensor::operator*(const Tensor& other) const {
    if (shape != other.shape) {
        throw runtime_error("las dimensiones son incompatibles para realizar la multiplicación");
    }
    vector<double> result_values(total_size);
    for (size_t i = 0; i < total_size; i++) {
        result_values[i] = data[i] * other.data[i];
    }
    return Tensor(shape, result_values);
}

Tensor Tensor::operator*(double scalar) const {
    vector<double> result_values(total_size);
    for (size_t i = 0; i < total_size; i++) {
        result_values[i] = data[i] * scalar;
    }
    return Tensor(shape, result_values);
}

// Metodos view y unsqueeze --------------
Tensor::Tensor(const vector<size_t>& shape, double* shared_data, size_t size)
    : shape(shape), total_size(size), data(shared_data), d_data(false) {
    size_t check_size = 1;
    for (size_t dim : shape) {
        check_size *= dim;
    }
    if (check_size != size) {
        throw runtime_error("dimensiones incompatibles en constructor de vista");
    }
}

Tensor Tensor::view(const vector<size_t>& new_shape) const {
    size_t new_total_size = 1;
    for (size_t dim : new_shape) {
        new_total_size *= dim;
    }
    if (new_total_size != total_size) {
        throw runtime_error("view() requiere el mismo número de elementos");
    }
    if (new_shape.size() > 3) {
        throw runtime_error("view() es máximo 3 dimensiones");
    }
    return Tensor(new_shape, data, total_size);
}

Tensor Tensor::unsqueeze(size_t dim) const {
    if (dim > shape.size()) {
        throw runtime_error("posición inválida para unsqueeze");
    }
    if (shape.size() + 1 > 3) {
        throw runtime_error("unsqueeze daría más de 3 dimensiones");
    }
    vector<size_t> new_shape;
    for (size_t i = 0; i < shape.size(); i++) {
        if (i == dim) {
            new_shape.push_back(1);
        }
        new_shape.push_back(shape[i]);
    }
    if (dim == shape.size()) {
        new_shape.push_back(1);
    }
    return Tensor(new_shape, data, total_size);
}

// Concatenacion-------------------
Tensor Tensor::concat(const vector<Tensor>& tensor, size_t dim) {
    if (tensor.empty()) {
        throw runtime_error("No hay tensores");
    }
    const Tensor& first = tensor[0];
    if (first.shape.size() != 2) {
        throw runtime_error("concat solo implementado para 2D");
    }
    if (dim >= 2) {
        throw runtime_error("dimensión invalida (0=filas, 1=columnas)");
    }
    for (size_t i = 1; i < tensor.size(); i++) {
        if (dim == 0) {
            if (tensor[i].shape[1] != first.shape[1]) {
                throw runtime_error("Número de columnas diferente");
            }
        } else {
            if (tensor[i].shape[0] != first.shape[0]) {
                throw runtime_error("Número de filas diferente");
            }
        }
    }
    vector<size_t> new_shape = first.shape;
    for (size_t i = 1; i < tensor.size(); i++) {
        new_shape[dim] += tensor[i].shape[dim];
    }
    size_t total_size = new_shape[0] * new_shape[1];
    vector<double> values(total_size);
    size_t current_pos = 0;
    for (const auto& t : tensor) {
        if (dim == 0) {
            size_t rows = t.shape[0];
            size_t cols = t.shape[1];
            for (size_t r = 0; r < rows; r++) {
                for (size_t c = 0; c < cols; c++) {
                    size_t src_idx = r * cols + c;
                    size_t dst_idx = (current_pos + r) * cols + c;
                    values[dst_idx] = t.data[src_idx];
                }
            }
            current_pos += rows;
        } else {
            size_t rows = t.shape[0];
            size_t cols = t.shape[1];
            size_t total_cols = new_shape[1];
            for (size_t r = 0; r < rows; r++) {
                for (size_t c = 0; c < cols; c++) {
                    size_t src_idx = r * cols + c;
                    size_t dst_idx = r * total_cols + current_pos + c;
                    values[dst_idx] = t.data[src_idx];
                }
            }
            current_pos += cols;
        }
    }
    return Tensor(new_shape, values);
}

//funciones amigas permitidas ----------
Tensor dot(const Tensor& a, const Tensor& b) {
    if (a.shape.size() != 1 || b.shape.size() != 1) {
        throw runtime_error("dot solo funciona con tensores 1D");
    }
    if (a.total_size != b.total_size) {
        throw runtime_error("Los vectores deben tener la misma longitud");
    }
    double resultado = 0.0;
    for (size_t i = 0; i < a.total_size; i++) {
        resultado += a.data[i] * b.data[i];
    }
    return Tensor({1}, {resultado});
}

Tensor matmul(const Tensor& a, const Tensor& b) {
    if (a.getShape().size() != 2 || b.getShape().size() != 2) {
        throw runtime_error("matmul solo funciona con tensores 2D");
    }

    size_t a_rows = a.shape[0];
    size_t a_cols = a.shape[1];
    size_t b_rows = b.shape[0];
    size_t b_cols = b.shape[1];

    if (a_cols != b_rows) {
        throw runtime_error("Dimensiones incompatibles para matmul (A.cols != B.rows)");
    }
    vector<double> resultado_values(a_rows * b_cols, 0.0);
    for (size_t i = 0; i < a_rows; i++) {
        for (size_t j = 0; j < b_cols; j++) {
            double sum = 0.0;
            for (size_t k = 0; k < a_cols; k++) {
                size_t a_idx = i * a_cols + k;
                size_t b_idx = k * b_cols + j;
                sum += a.data[a_idx] * b.data[b_idx];
            }
            size_t resultado_idx = i * b_cols + j;
            resultado_values[resultado_idx] = sum;
        }
    }
    return Tensor({a_rows, b_cols}, resultado_values);
}

//Redes Neuronales -------------- (ver el main)
void print_shape(const Tensor& t, const string& name) {
    cout << name << " shape: [";
    for (size_t d : t.getShape()) {
        cout << d << " ";
    }
    cout << "]" << endl;
}


int main() {
    //Tensor A = Tensor::arange(2, 5);
    //cout << "original: ";
    //A.print();
    //ReLU relu;
    //Tensor B = A.apply(relu);
    //cout << "despues de ReLU: ";
    //B.print();
    //Sigmoid sigmoid;
    //Tensor C = A.apply(sigmoid);
    //cout << "despues de Sigmoid: ";
    //C.print();
    //ReLU relu2;
    //Tensor D = A.apply(relu2).apply(sigmoid);
    //Tensor B = Tensor::ones({3});
    //Tensor C = A + B;
    //Tensor D = A - B;
    //Tensor E = A * B;
    //Tensor F = A * 2.0;
    //Tensor H = (A + B) * 2.0;
    //A.print();
    //B.print();
    //C.print();
    //D.print();
    //E.print();
    //F.print();
    //H.print();
    //Tensor A = Tensor::arange(0, 12);
    //A.print();
    //Tensor B = A.view({3, 4});
    //B.print();
    //Tensor C = A.view({2, 2, 3});
    //C.print();
    //Tensor D = A.unsqueeze(0);
    //Tensor E = B.unsqueeze(1);
    //Tensor F = B.unsqueeze(2);
    //Tensor A = Tensor::ones({2, 3});
    //Tensor B = Tensor::zeros({2, 3});
    //Tensor C = Tensor::concat({A, B}, 0);
    //Tensor D = Tensor::concat({A, B}, 1);
    //Tensor E = Tensor::concat({A, A, B, B}, 0);
    //C.print();
    //D.print();
    //E.print();
    // Producto punto
    //Tensor v = Tensor::arange(1, 5); // [1,2,3,4]
    //Tensor w = Tensor::arange(5, 9); // [5,6,7,8]
    //Tensor dp = dot(v, w);               // 1*5+2*6+3*7+4*8 = 70
    //dp.print();

    // Multiplicación matricial
    //Tensor M1 = Tensor::random({3, 4}, -1, 1); // 3×4
    //Tensor M2 = Tensor::random({4, 5}, -1, 1); // 4×5
    //Tensor M3 = matmul(M1, M2); // 3×5
    //M3.print();
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

