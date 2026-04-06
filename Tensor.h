//
// Created by alice on 5/4/2026.
//

#ifndef TAREA1_TENSOR_H
#define TAREA1_TENSOR_H

#include <vector>
#include <cstddef>


class Tensor {
private:
    double* data;
    std::vector<size_t> shape;
    size_t total_size;
    public:
    Tensor ( const std :: vector < size_t >& shape , const std :: vector < double >& values );

    ~Tensor();

    static Tensor zeros(const std::vector<size_t>& shape);
    static Tensor ones(const std::vector<size_t>& shape);
    static Tensor random(const std::vector<size_t>& shape, double min, double max);
    static Tensor arange (double start , double end );

    void print() const;


}


#endif //TAREA1_TENSOR_H