// DataStructure.hpp

#ifndef DATA_STRUCTURE_HPP
#define DATA_STRUCTURE_HPP

#include <vector>
#include "Vector.hpp"

/**
 * @struct QueryResult
 * @brief Encapsula os resultados de uma busca por similaridade.
 * * Armazena os vizinhos encontrados e as métricas de desempenho da busca,
 * como o número de comparações de distância realizadas.
 */
struct QueryResult {
    // Vetor com os vizinhos mais próximos encontrados
    std::vector<FeatureVector> neighbors;
    // Esta é uma métrica chave para a análise de custo prático.
    int comparisons = 0;
};


/**
 * @class DataStructure
 * @brief Classe base abstrata (interface) para todas as estruturas de dados.
 * * Define um "contrato" que todas as estruturas (Lista, Quadtree, Hash) devem seguir.
 * Isso garante que todas terão os mesmos métodos públicos, facilitando a comparação
 * entre elas no programa principal.
 */
class DataStructure {
public:
    // Destrutor virtual para garantir que as classes filhas sejam destruídas corretamente.
    virtual ~DataStructure() {}

    /**
     * @brief (Virtual Pura) Insere um novo vetor de características na estrutura de dados.
     * @param vec O FeatureVector a ser inserido.
     */
    virtual void insert(const FeatureVector& vec) = 0;

    /**
     * @brief (Virtual Pura) Busca os 'k' vetores mais similares a um vetor de consulta.
     * @param query_vec O vetor de referência para a busca.
     * @param k O número de vizinhos mais próximos a serem retornados.
     * @return Um objeto QueryResult contendo os vizinhos e as estatísticas da busca.
     */
    virtual QueryResult query(const FeatureVector& query_vec, int k) = 0;
};

#endif // DATA_STRUCTURE_HPP
