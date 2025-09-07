// ----------------------------------------------------------------------------
// Este programa é o executor principal do experimento de análise de algoritmos.
// Ele foi projetado para testar o desempenho de uma estrutura de dados de busca
// por similaridade. O fluxo é o seguinte:
// 1. Carrega os vetores de características de imagens de um arquivo 'dataset.csv'.
// 2. Insere todos esses vetores em uma estrutura de dados (da classe 'Lista').
// 3. Seleciona algumas imagens do próprio dataset para servirem como consulta.
// 4. Para cada consulta, mede o tempo de busca e o número de comparações.
// 5. Salva essas métricas em um arquivo de resultados 'results.csv'.
// ----------------------------------------------------------------------------

//Bibliotecas necessárias
#include <iostream>
#include <vector> 
#include <string>
#include <fstream>
#include <sstream>
#include <chrono>
#include <numeric>
#include <algorithm>

// Arquivos do Projeto
#include "Vector.hpp"          // Define o que é um FeatureVector
#include "DataStructure.hpp"   // Define a interface que a Lista deve seguir
#include "Lista.hpp"           // Implementação da Lista
#include "Hash.hpp"
#include "Quadtree.hpp"
/**
 * @brief Função auxiliar para carregar o dataset de um arquivo CSV.
 * @param filename O nome do arquivo CSV a ser lido (ex: "dataset.csv").
 * @return Um std::vector contendo todos os FeatureVectors lidos do arquivo.
 */
std::vector<FeatureVector> loadDatasetFromFile(const std::string& filename) {
    std::vector<FeatureVector> dataset;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "ERRO FATAL: Nao foi possivel abrir o arquivo de dataset '" << filename << "'." << std::endl;
        std::cerr << "Certifique-se de que o arquivo existe e esta na mesma pasta do executavel." << std::endl;
        return dataset; // Retorna o vetor vazio para indicar o erro
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line.rfind("//", 0) == 0) continue;

        std::stringstream ss(line);
        std::string value;
        FeatureVector vec;
        
        std::getline(ss, value, ','); vec.image_id = std::stoi(value);
        std::getline(ss, value, ','); vec.r = std::stod(value);
        std::getline(ss, value, ','); vec.g = std::stod(value);
        std::getline(ss, value); vec.b = std::stod(value);
        
        dataset.push_back(vec);
    }
    
    file.close();
    return dataset;
}

// Ponto de entrada do programa
int main() {
    // CARREGAR O DATASET
    std::cout << ">> Iniciando experimento..." << std::endl;
    std::cout << "1. Carregando vetores do arquivo 'dataset.csv'..." << std::endl;
    std::vector<FeatureVector> dataset = loadDatasetFromFile("dataset.csv");

    if (dataset.empty()) {
        std::cerr << "!! Experimento abortado: o dataset nao pode ser carregado." << std::endl;
        return 1; // Erro
    }
    std::cout << "   -> " << dataset.size() << " vetores carregados com sucesso." << std::endl << std::endl;

    // PREPARAR A ESTRUTURA DE DADOS
    std::cout << "2. Inserindo vetores na sua estrutura de dados 'Lista'..." << std::endl;
    
    Lista list_structure;
    
    for (const auto& vec : dataset) {
        list_structure.insert(vec);
    }
    std::cout << "   -> Insercao concluida." << std::endl << std::endl;

    // PREPARAR A ESTRUTURA DE DADOS HASH
    std::cout << "2.1 Inserindo vetores na sua estrutura de dados 'Hash' ..." << std::endl;
    HashTable hash_structure(1013, 5, 25); //quantidade de buckets
    for(const auto& vec : dataset){
        hash_structure.insert(vec);
    }
    std::cout << "  -> Insercao Hash concluida" << std::endl << std::endl;

    // PREPARAR A ESTRUTURA DE DADOS QUADTREE
    std::cout << "2.1 Inserindo vetores na sua estrutura de dados 'Quadtree' ..." << std::endl;
    Quadtree quad_structure; // conforme sua Quadtree.hpp
    for (const auto& vec : dataset) {
        quad_structure.insert(vec);
    }
    std::cout << "  -> Insercao Quadtree concluida" << std::endl << std::endl;

    // PREPARAR O ARQUIVO DE SAÍDA
    std::string results_filename = "results.csv";
    std::ofstream results_file(results_filename);
    
    results_file << "estrutura,query_image_id,tempo_busca_ms,comparacoes,query_r,query_g,query_b,top_k_avg_similarity\n";
    std::cout << "3. Arquivo de resultados '" << results_filename << "' preparado." << std::endl << std::endl;

    // EXECUTAR OS EXPERIMENTOS DE BUSCA (Lista)
    std::cout << "4.0 Executando as buscas por similaridade (Lista)..." << std::endl;
    int k = 5; // O número de vizinhos mais próximos que queremos encontrar
    int num_queries = std::min((k*2), (int)dataset.size()); // Testaremos com as k*2 primeiras imagens

    for (int i = 0; i < num_queries; ++i) {
        const FeatureVector& query_vec = dataset[i];

        // Inicio da medição de tempo busca
        auto start_time = std::chrono::high_resolution_clock::now();
        
        QueryResult result = list_structure.query(query_vec, k);
        
        // Fim da medição de tempo busca
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration_ms = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(end_time - start_time);

        // Calcula a similaridade média dos vizinhos encontrados
        double total_similarity = 0.0;
        if (!result.neighbors.empty()) {
            for (const auto& neighbor : result.neighbors) {
                total_similarity += query_vec.similarityTo(neighbor);
            }
            total_similarity /= result.neighbors.size();
        }

        // Grava a linha de resultado no arquivo CSV
        results_file <<"Lista,"
                     << query_vec.image_id << ","
                     << duration_ms.count() << ","
                     << result.comparisons << ","
                     << query_vec.r << ","
                     << query_vec.g << ","
                     << query_vec.b << ","
                     << total_similarity << "\n";
        
        std::cout << "   -> Consulta com ID " << query_vec.image_id << " concluida. (" 
                  << duration_ms.count() << " ms, " 
                  << result.comparisons << " comparacoes)" << std::endl;
    }

    // EXECUTAR OS EXPERIMENTOS DE BUSCA (Hash)
    std::cout << "\n4.1 Executanto as buscas por similaridade (Hash)..." << std::endl;
    for (int i = 0; i < num_queries; ++i){
        const FeatureVector& query_vec = dataset[i];

        auto start_time = std::chrono::high_resolution_clock::now();
        QueryResult result = hash_structure.query(query_vec, k);
        auto end_time = std::chrono::high_resolution_clock::now();

        auto duration_ms = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(end_time - start_time);
        double total_similarity = 0.0;
        if (!result.neighbors.empty()){
            for(const auto& neighbor : result.neighbors){
                total_similarity += query_vec.similarityTo(neighbor);
            }
            total_similarity /= result.neighbors.size();
        }

        results_file << "Hash,"
                << query_vec.image_id << ","
                << duration_ms.count() << ","
                << result.comparisons << ","
                << query_vec.r << ","
                << query_vec.g << ","
                << query_vec.b << ","
                << total_similarity << "\n";

            std::cout << "   -> Consulta com ID " << query_vec.image_id
            << " concluida. (" << duration_ms.count()
            << " ms, " << result.comparisons << " comparacoes)" << std::endl;
    }

    // EXECUTAR OS EXPERIMENTOS DE BUSCA (Quadtree)
    std::cout << "\n4.1 Executanto as buscas por similaridade (Quadtree)..." << std::endl;
    for (int i = 0; i < num_queries; ++i){
        const FeatureVector& query_vec = dataset[i];

        auto start_time = std::chrono::high_resolution_clock::now();
        QueryResult result = quad_structure.query(query_vec, k);
        auto end_time = std::chrono::high_resolution_clock::now();

        auto duration_ms = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(end_time - start_time);
        double total_similarity = 0.0;
        if (!result.neighbors.empty()){
            for(const auto& neighbor : result.neighbors){
                total_similarity += query_vec.similarityTo(neighbor);
            }
            total_similarity /= result.neighbors.size();
        }

        results_file << "Quadtree,"
                << query_vec.image_id << ","
                << duration_ms.count() << ","
                << result.comparisons << ","
                << query_vec.r << ","
                << query_vec.g << ","
                << query_vec.b << ","
                << total_similarity << "\n";

            std::cout << "   -> Consulta com ID " << query_vec.image_id
            << " concluida. (" << duration_ms.count()
            << " ms, " << result.comparisons << " comparacoes)" << std::endl;
    }

    results_file.close();
    std::cout << "\n>> Experimentos finalizados com sucesso!" << std::endl;
    std::cout << "   Resultados salvos em '" << results_filename << "'." << std::endl;

    return 0;
}
