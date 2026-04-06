#include <iostream>
#include "Tensor.h"
#include <cmath>
using namespace std;

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

Tensor::Tensor(const std::vector<size_t>& shape, const std::vector<double>&values) : shape(shape) {
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
    delete[] data;
}

//copia
Tensor::Tensor(const Tensor& other)
    : shape(other.shape), total_size(other.total_size) {
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
      shape(std::move(other.shape)),
      total_size(other.total_size) {
    other.data = nullptr;
    other.total_size = 0;
}

//a. mov
Tensor& Tensor::operator=(Tensor&& other) noexcept {
    if (this == &other) return *this;

    delete[] data;
    data = other.data;
    shape = std::move(other.shape);
    total_size = other.total_size;

    other.data = nullptr;
    other.total_size = 0;

    return *this;
}

Tensor Tensor::zeros(const std::vector<size_t>& shape) {
    size_t total_size = 1;
    for (size_t dim : shape) {
        total_size *= dim;
    }
    std::vector<double> values(total_size, 0.0);
    return Tensor(shape, values);
}

Tensor Tensor::ones(const std::vector<size_t>& shape) {
    size_t total_size = 1;
    for (size_t dim : shape) {
        total_size *= dim;
    }

    vector<double> values(total_size, 1.0);
    return Tensor(shape, values);
}

Tensor Tensor::random(const std::vector<size_t>& shape, double min, double max) {
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
    if (shape != other.shape) {
        throw runtime_error("las dimensiones son incompatibles para realizar la suma");
    }
    vector<double> result_values(total_size);
    for (size_t i = 0; i < total_size; i++) {
        result_values[i] = data[i] + other.data[i];
    }
    return Tensor(shape, result_values);
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