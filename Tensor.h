//
// Created by alice on 5/4/2026.
//

#ifndef TAREA1_TENSOR_H
#define TAREA1_TENSOR_H

#include <vector>
#include <cstddef>

class TensorTransform;

class Tensor {
private:
    double* data;
    std::vector<size_t> shape;
    size_t total_size;
    bool d_data;
    public:
    Tensor ( const std :: vector < size_t >& shape , const std :: vector < double >& values );

    ~Tensor();

    Tensor(const Tensor& other);
    Tensor& operator=(const Tensor& other);

    Tensor(Tensor&& other) noexcept;
    Tensor& operator=(Tensor&& other) noexcept;

    const std::vector<size_t>& getShape() const { return shape; }
    size_t getTotalSize() const { return total_size; }
    const double* getData() const { return data; }

    Tensor apply(const TensorTransform& transform) const;

    static Tensor zeros(const std::vector<size_t>& shape);
    static Tensor ones(const std::vector<size_t>& shape);
    static Tensor random(const std::vector<size_t>& shape, double min, double max);
    static Tensor arange (double start , double end );

    Tensor operator+(const Tensor& other) const;
    Tensor operator-(const Tensor& other) const;
    Tensor operator*(const Tensor& other) const;
    Tensor operator*(double scalar) const;

    Tensor view(const std::vector<size_t>& new_shape) const;
    Tensor unsqueeze(size_t dim) const;
    Tensor(const std::vector<size_t>& shape, double* shared_data, size_t size);

    static Tensor concat(const std::vector<Tensor>& tensors, size_t dim);

    friend Tensor dot(const Tensor& a, const Tensor& b);
    friend Tensor matmul(const Tensor& a, const Tensor& b);


    void print() const;
};





#endif //TAREA1_TENSOR_H