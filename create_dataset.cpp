// create_dataset.cpp

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem> // Requer C++17 para iterar em diretórios

// Define que este arquivo .cpp irá conter a implementação da biblioteca stb_image.
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// A função main não precisa mais de 'argc' e 'argv'
int main() {
    // Defina o caminho para a pasta com suas imagens diretamente no código.
    std::filesystem::path dataset_root_path = "./database_flowers";

    // Defina o nome do arquivo de saída.
    std::string output_filename = "dataset.csv";


    std::ofstream output_file(output_filename);
    if (!output_file.is_open()) {
        std::cerr << "Erro: Nao foi possivel criar o arquivo de saida " << output_filename << std::endl;
        return 1;
    }

    std::cout << "Processando imagens do diretorio raiz: " << dataset_root_path << std::endl;
    std::cout << "Salvando vetores em: " << output_filename << std::endl;

    int image_id_counter = 1;

    // Itera recursivamente por todos os arquivos e pastas a partir do caminho raiz.
    for (const auto& entry : std::filesystem::recursive_directory_iterator(dataset_root_path)) {
        if (!entry.is_regular_file()) {
            continue; // Pula se não for um arquivo (ex: é um diretório)
        }

        const auto& path = entry.path();
        std::string extension = path.extension().string();

        if (extension == ".jpg" || extension == ".jpeg" || extension == ".png" || extension == ".bmp") {
            int width, height, channels;
            unsigned char* img_data = stbi_load(path.string().c_str(), &width, &height, &channels, 0);

            if (img_data == nullptr) {
                std::cerr << "Aviso: Nao foi possivel carregar a imagem " << path.string() << std::endl;
                continue;
            }

            if (channels < 3) {
                std::cerr << "Aviso: Imagem " << path.string() << " nao e colorida (RGB). Pulando." << std::endl;
                stbi_image_free(img_data);
                continue;
            }

            unsigned long long total_r = 0, total_g = 0, total_b = 0;
            long num_pixels = width * height;

            for (int i = 0; i < num_pixels; ++i) {
                total_r += img_data[i * channels + 0];
                total_g += img_data[i * channels + 1];
                total_b += img_data[i * channels + 2];
            }

            double avg_r = static_cast<double>(total_r) / num_pixels;
            double avg_g = static_cast<double>(total_g) / num_pixels;
            double avg_b = static_cast<double>(total_b) / num_pixels;

            output_file << image_id_counter << "," << avg_r << "," << avg_g << "," << avg_b << "\n";
            std::cout << "Processado: " << path.filename() << " -> ID: " << image_id_counter << std::endl;

            image_id_counter++;
            stbi_image_free(img_data);
        }
    }

    output_file.close();
    std::cout << "\nDataset criado com sucesso em " << output_filename << std::endl;

    return 0;
}