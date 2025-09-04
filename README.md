# Análise de Estruturas de Dados para Busca de Similaridade em Imagens

Trabalho prático da disciplina de Projeto e Análise de Algoritmos (2025/2)  [cite_start]do curso de Ciência da Computação da Pontifícia Universidade Católica de Minas Gerais (PUC Minas)[cite: 1, 2].

**Professor:** Silvio Jamil F. Guimarães 

---

## 📄 Descrição do Projeto

Este projeto tem como objetivo realizar uma análise de custos computacionais, teórica e prática, de diferentes estruturas de dados aplicadas a um problema de busca por similaridade em um banco de dados de imagens. Cada imagem é representada por um vetor de características (cor RGB média), e o objetivo é encontrar as imagens mais "parecidas" com uma imagem de consulta.

## ✨ Funcionalidades

-   **Processamento de Imagens:** Um utilitário em C++ que lê um diretório de imagens (`.jpg`, `.png`, etc.) e extrai um vetor de características (RGB médio) para cada uma, salvando em um arquivo `.csv`.
-   **Busca Top-K:** Implementação de algoritmos para encontrar os *k* vizinhos mais próximos de um vetor de consulta.
-   **Benchmarking:** O programa principal mede métricas de desempenho para cada busca, como tempo de execução (em milissegundos) e número de comparações de distância.
-   **Geração de Relatório:** Os resultados dos experimentos são salvos em um arquivo `results.csv` para fácil análise e criação de gráficos.

## implemented-structures Estruturas de Dados Implementadas

O trabalho completo envolve a análise das seguintes estruturas:

-   [x] **Lista Duplamente Encadeada:** Implementação manual. (Status: Concluído)
-   [ ] **Quadtree/Octree:** (Status: A implementar)
-   [ ] **Tabela Hash (LSH):** (Status: A implementar)

## 🚀 Como Compilar e Executar

Siga os passos abaixo para compilar e rodar o projeto.

### Pré-requisitos

-   Um compilador C++ moderno (g++ recomendado, com suporte a C++17).
-   Uma pasta contendo as imagens do dataset para análise.

### Passo 1: Preparação do Dataset

1.  **Baixe o dataset "Flowers Recognition" do Kaggle:**
    * **Link:** [https://www.kaggle.com/datasets/alxmamaev/flowers-recognition](https://www.kaggle.com/datasets/alxmamaev/flowers-recognition)

2.  **Organize os arquivos:**
    * Descompacte o conteúdo baixado.
    * Crie uma pasta chamada `database_flowers` na raiz do seu projeto.
    * Mova todas as pastas de flores (`daisy`, `rose`, `sunflower`, etc.) para dentro de `database_flowers`.

    A estrutura de pastas final deve ser parecida com esta:
    ```
    /seu-projeto/
    |-- database_flowers/
    |   |-- daisy/
    |   |   |-- 100080576_f52e8ee070_n.jpg
    |   |   |-- ...
    |   |-- rose/
    |   |-- ...
    |-- create_dataset.cpp
    |-- main.cpp
    |-- ... (outros arquivos)
    ```

    > **Nota:** Se você preferir usar um nome de pasta diferente ou um caminho em outro local, lembre-se de ajustar o comando no Passo 3.

### Passo 2: Compilação

Compile os dois programas principais usando os seguintes comandos no terminal:

```bash
# Compila o programa que gera o dataset a partir das imagens
g++ create_dataset.cpp -o create_dataset -std=c++17

# Compila o programa principal que roda os experimentos
g++ main.cpp -o meu_programa -std=c++17
```

### Passo 3: Geração do Dataset

Use o programa `create_dataset` para processar as imagens da pasta que você preparou.

```bash
# O comando lê de 'database_flowers' e cria 'dataset.csv'
./create_dataset database_flowers dataset.csv
```

### Passo 4: Execução do Experimento

Execute o programa principal para realizar as buscas e gerar o relatório.

```bash
./meu_programa
```

### Passo 5: Análise dos Resultados

Após a execução, um arquivo chamado `results.csv` será criado no diretório, contendo as métricas de desempenho para cada busca realizada.

## 👥 Membros do Grupo

-   **Ana Cristina Martins Silva**
-   **Kenia Teixeira de Paula Costa**
-   **Letícia Azevedo Cota Barbosa**
-   **Livia Alves Ferreira**
-   **Luiz Fernando ANtunes da Silva Frassi**
-   **Pedro Augusto Gomes Ferreira de Albuquerque**
-   **Pedro Henrique Gaioso de Oliveira**
