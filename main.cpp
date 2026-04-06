#include <iostream>
#include "Tensor.h"
using namespace std;

Tensor::Tensor(const std::vector<size_t>& shape, const std::vector<double>&values) : shape(shape) {
    total_size = 1;
    for (size_t dim : shape) {
        total_size *= dim;
    }

    if (values.size() != total_size) {
        cout << "El número de valores no coincide con la forma del tensor" << endl;
    }

    data = new double[total_size];

    for (size_t i = 0; i < total_size; i++) {
        data[i] = values[i];
    }
}

Tensor::~Tensor() {
    delete[] data;
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

    std::vector<double> values(total_size, 1.0);
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

    std::vector<double> values(total_size);
    for (size_t i = 0; i < total_size; i++) {
        double r = (double)rand()/RAND_MAX;
        values[i] = min+r*(max-min);
    }
    return Tensor(shape, values);
}

Tensor Tensor::arange(double start, double end) {
    size_t size = static_cast<size_t> (end-start);
    std::vector<double> values(size);
    for (size_t i = 0; i < size; i++) {
        values[i] = start + i;
    }
    return Tensor({size}, values);
}


int main(){

    return 0;
}