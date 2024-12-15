
template <typename FLOAT, std::size_t N>
Vector<FLOAT, N> & SquareMatrix<FLOAT,N>::operator[](std::size_t i) {
    return matrix[i];
}

template <typename FLOAT, std::size_t N>
Vector<FLOAT, N> SquareMatrix<FLOAT,N>::operator[](std::size_t i) const {
    return matrix[i];
}

template <typename FLOAT, std::size_t N>
FLOAT SquareMatrix<FLOAT, N>::at(size_t row, size_t column) const {
    return matrix[column][row];
}

template <typename FLOAT, std::size_t N>
FLOAT& SquareMatrix<FLOAT, N>::at(size_t row, size_t column) {
    return matrix[column][row];
}

template <typename FLOAT, std::size_t N>
Vector<FLOAT, N> SquareMatrix<FLOAT, N>::operator*(const Vector<FLOAT, N> vector) {
    Vector<FLOAT, N> result;
    for (std::size_t i = 0; i < N; ++i) {
        result[i] = 0;
        for (std::size_t j = 0; j < N; ++j) {
            result[i] += at(i, j) * vector[j];
        }
    }
    return result;
}

template <typename FLOAT, std::size_t N>
SquareMatrix<FLOAT, N> operator*(const SquareMatrix<FLOAT, N> matrix1, const SquareMatrix<FLOAT, N> matrix2) {
    SquareMatrix<FLOAT, N> result;
    for (std::size_t i = 0; i < N; ++i) {
        for (std::size_t j = 0; j < N; ++j) {
            result.at(i, j) = 0;
            for (std::size_t k = 0; k < N; ++k) {
                result.at(i, j) += matrix1.at(i, k) * matrix2.at(k, j);
            }
        }
    }
    return result;
}
