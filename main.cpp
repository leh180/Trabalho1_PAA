// ----------------------------------------------------------------------------
// Executor principal dos experimentos: Lista, Hash e Quadtree
// ----------------------------------------------------------------------------

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <chrono>
#include <numeric>
#include <algorithm>

#include "Vector.hpp"        // Define FeatureVector (image_id, r, g, b, distanceTo/similarityTo)
#include "DataStructure.hpp" // Interface comum
#include "Lista.hpp"         // Implementação: Lista
#include "Hash.hpp"          // Implementação: Hash
#include "Quadtree.hpp"      // Implementação: Quadtree

// Carrega dataset.csv
std::vector<FeatureVector> loadDatasetFromFile(const std::string& filename) {
    std::vector<FeatureVector> dataset;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "ERRO FATAL: Nao foi possivel abrir o arquivo de dataset '" << filename << "'.\n";
        std::cerr << "Certifique-se de que o arquivo existe e esta na mesma pasta do executavel.\n";
        return dataset;
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
        std::getline(ss, value);      vec.b = std::stod(value);

        dataset.push_back(vec);
    }

    file.close();
    return dataset;
}

int main() {
    // --- ETAPA 1: CARREGAR O DATASET ---
    std::cout << ">> Iniciando experimento...\n";
    std::cout << "1. Carregando vetores do arquivo 'dataset.csv'...\n";
    std::vector<FeatureVector> dataset = loadDatasetFromFile("dataset.csv");

    if (dataset.empty()) {
        std::cerr << "!! Experimento abortado: o dataset nao pode ser carregado.\n";
        return 1;
    }
    std::cout << "   -> " << dataset.size() << " vetores carregados com sucesso.\n\n";

    // --- ETAPA 2.0: LISTA ---
    std::cout << "2. Inserindo vetores na estrutura 'Lista'...\n";
    Lista list_structure;
    for (const auto& vec : dataset) {
        list_structure.insert(vec);
    }
    std::cout << "   -> Insercao (Lista) concluida.\n\n";

    // --- ETAPA 2.1: HASH ---
    std::cout << "2.1 Inserindo vetores na estrutura 'Hash'...\n";
    HashTable hash_structure(1013, 5, 25); // ajuste conforme sua Hash.hpp
    for (const auto& vec : dataset) {
        hash_structure.insert(vec);
    }
    std::cout << "   -> Insercao (Hash) concluida.\n\n";

    // --- ETAPA 2.2: QUADTREE ---
    std::cout << "2.2 Inserindo vetores na estrutura 'Quadtree'...\n";
    Quadtree quad_structure; // conforme sua Quadtree.hpp
    for (const auto& vec : dataset) {
        quad_structure.insert(vec);
    }
    std::cout << "   -> Insercao (Quadtree) concluida.\n\n";

    // --- ETAPA 3: ARQUIVO DE SAIDA ---
    std::string results_filename = "results.csv";
    std::ofstream results_file(results_filename);
    results_file << "estrutura,query_image_id,tempo_busca_ms,comparacoes,query_r,query_g,query_b,top_k_avg_similarity\n";
    std::cout << "3. Arquivo de resultados '" << results_filename << "' preparado.\n\n";

    // --- Parâmetros de busca ---
    int k = 5;
    int num_queries = std::min(k * 2, (int)dataset.size()); // k*2 primeiras imagens

    // Helper para rodar e registrar um experimento
    auto run_and_log = [&](const char* nome, DataStructure& ds) {
        std::cout << "4.x Executando buscas por similaridade (" << nome << ")...\n";
        for (int i = 0; i < num_queries; ++i) {
            const FeatureVector& query_vec = dataset[i];

            auto start_time = std::chrono::high_resolution_clock::now();
            QueryResult result = ds.query(query_vec, k);
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration_ms = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(end_time - start_time);

            double total_similarity = 0.0;
            if (!result.neighbors.empty()) {
                for (const auto& neighbor : result.neighbors) {
                    total_similarity += query_vec.similarityTo(neighbor);
                }
                total_similarity /= result.neighbors.size();
            }

            results_file << nome << ","
                         << query_vec.image_id << ","
                         << duration_ms.count() << ","
                         << result.comparisons << ","
                         << query_vec.r << ","
                         << query_vec.g << ","
                         << query_vec.b << ","
                         << total_similarity << "\n";

            std::cout << "   -> Consulta ID " << query_vec.image_id
                      << " (" << duration_ms.count() << " ms, "
                      << result.comparisons << " comparacoes)\n";
        }
        std::cout << "   -> (" << nome << ") concluido.\n\n";
    };

    // --- ETAPA 4: RODAR LISTA, HASH, QUADTREE ---
    run_and_log("Lista",     list_structure);
    run_and_log("Hash",      hash_structure);
    run_and_log("Quadtree",  quad_structure);

    results_file.close();
    std::cout << "\n>> Experimentos finalizados com sucesso!\n";
    std::cout << "   Resultados salvos em '" << results_filename << "'.\n";
    return 0;
}
