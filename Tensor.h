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



}


#endif //TAREA1_TENSOR_H