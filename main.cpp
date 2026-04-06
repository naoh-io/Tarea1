#include <iostream>
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



int main(){
    cout << "hola mundo";
    return 0;
}