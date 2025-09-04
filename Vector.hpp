// Vector.hpp

#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <cmath>      // Para funções matemáticas como sqrt e pow
#include <iostream>   // Para permitir a impressão (ostream)

/**
 * @struct FeatureVector
 * @brief Representa um único "documento" (neste caso, uma imagem) como um vetor de características.
 * * Para imagens coloridas, usamos os valores médios dos canais Vermelho (R), Verde (G) e Azul (B).
 * Cada vetor também armazena um ID para identificar a imagem original.
 */
struct FeatureVector {
    // Membros da struct que armazenam os dados do vetor
    double r, g, b; // Valores médios dos canais de cor (0-255)
    int image_id;   // Identificador único da imagem

    /**
     * @brief Calcula a DISTÂNCIA DO COSSENO entre este vetor e outro.
     * @details A distância do cosseno mede a dissimilaridade angular (0 = mesma direção, 2 = direções opostas).
     * Quanto menor o valor, mais parecidos são os vetores em termos de orientação.
     * A fórmula é: 1 - Similaridade de Cosseno.
     * @param other O outro FeatureVector para comparar.
     * @return A Distância do Cosseno (double, entre 0 e 2).
     */
    double distanceTo(const FeatureVector& other) const {
        // 1. Calcular o produto escalar (dot product)
        double dot_product = (this->r * other.r) + (this->g * other.g) + (this->b * other.b);

        // 2. Calcular a magnitude (norma) de cada vetor
        double mag_this = std::sqrt((this->r * this->r) + (this->g * this->g) + (this->b * this->b));
        double mag_other = std::sqrt((other.r * other.r) + (other.g * other.g) + (other.b * other.b));

        // Evitar divisão por zero se um dos vetores for nulo
        if (mag_this == 0.0 || mag_other == 0.0) {
            return 1.0; // Retorna 1.0 (dissimilaridade neutra) se um vetor for nulo
        }

        // 3. Calcular a similaridade de cosseno
        double similarity = dot_product / (mag_this * mag_other);

        // 4. Retornar a distância do cosseno
        return 1.0 - similarity;
    }

    /**
     * @brief Calcula a SIMILARIDADE DE COSSENO (entre -1 e 1) com outro vetor.
     * @details Converte a distância do cosseno de volta para similaridade.
     * Um valor de 1.0 significa que os vetores têm a mesma orientação (máxima similaridade).
     * Um valor de 0 significa que são ortogonais.
     * @param other O outro FeatureVector para comparar.
     * @return A pontuação de similaridade de cosseno (double).
     */
    double similarityTo(const FeatureVector& other) const {
        // A similaridade é o inverso da distância do cosseno
        // Similaridade = 1 - Distância
        double dist = distanceTo(other);
        return 1.0 - dist;
    }
};

/**
 * @brief Sobrecarga do operador de saída (<<) para imprimir um FeatureVector de forma legível.
 * (Esta função não precisa de alterações)
 */
std::ostream& operator<<(std::ostream& os, const FeatureVector& vec) {
    os << "Image ID: " << vec.image_id << " (R=" << vec.r << ", G=" << vec.g << ", B=" << vec.b << ")";
    return os;
}

#endif // VECTOR_HPP