#ifndef LISTA_HPP
#define LISTA_HPP

#include <iostream>
#include <vector>
#include <algorithm>
#include <utility>

#include "DataStructure.hpp" // Inclui a interface que precisamos seguir

class No
{
public:
    FeatureVector imagem;
    No *prox;
    No *ant;

    No()
    {
        prox = nullptr;
        ant = nullptr;
    }

    No(const FeatureVector &i)
    {
        imagem = i;
        prox = nullptr;
        ant = nullptr;
    }
};

// Classe Lista agora herda de DataStructure
class Lista : public DataStructure
{
private:
    No *primeiro; // nó cabeça
    No *ultimo;

public:
    // Construtor
    Lista()
    {
        primeiro = new No(); // nó cabeça
        ultimo = primeiro;
    }

    // ADICIONADO: Limpar a memória
    ~Lista()
    {
        No *atual = primeiro;
        while (atual != nullptr)
        {
            No *proximo_no = atual->prox;
            delete atual;
            atual = proximo_no;
        }
    }

    void insert(const FeatureVector &vec) override
    {
        inserirFinal(vec);
    }

    QueryResult query(const FeatureVector &query_vec, int k) override
    {
        QueryResult result;
        if (listaVazia() || k == 0)
        {
            return result;
        }

        std::vector<std::pair<double, FeatureVector>> all_distances;

        No *atual = primeiro->prox;
        while (atual != nullptr)
        {
            double dist = query_vec.distanceTo(atual->imagem); // Usa a variável 'imagem'
            all_distances.push_back({dist, atual->imagem});
            result.comparisons++;

            atual = atual->prox;
        }

        std::sort(all_distances.begin(), all_distances.end(),
                  [](const auto &a, const auto &b)
                  {
                      return a.first < b.first;
                  });

        int num_results = std::min(k, (int)all_distances.size());
        for (int i = 0; i < num_results; ++i)
        {
            result.neighbors.push_back(all_distances[i].second);
        }

        return result;
    }

    // inserir no inicio da lista, configurando os ponteiros
    void inserirInicio(FeatureVector i)
    {
        No *novo = new No(i);
        novo->prox = primeiro->prox;
        novo->ant = primeiro;
        if (primeiro == ultimo)
        {
            ultimo = novo;
        }
        else
        {
            primeiro->prox->ant = novo;
        }
        primeiro->prox = novo;
    }

    // Inserir no fim da lista
    void inserirFinal(const FeatureVector &i)
    {
        if (listaVazia())
        {
            inserirInicio(i);
        }
        else
        {
            No *novo = new No(i);
            novo->ant = ultimo;
            novo->prox = nullptr;
            ultimo->prox = novo;
            ultimo = novo;
        }
    }

    void removerInicio()
    {
        // testar se dá pra remover
        if (listaVazia())
        {
            std::cout << "Lista vazia, não é possível realizar a remoção" << std::endl;
        }
        else
        {
            No *removido = primeiro->prox;
            if (removido == ultimo)
            {
                // caso de apenas um elemento
                primeiro->prox = nullptr;
                ultimo = primeiro;
                delete removido;
            }
            else
            {
                // caso genérico
                primeiro->prox = removido->prox;
                primeiro->prox->ant = primeiro;
                delete removido;
            }
        }
    }

    void removerFinal()
    {
        if (listaVazia())
        {
            std::cout << "Lista vazia, não é possível realizar a remoção" << std::endl;
        }
        else if (primeiro->prox == ultimo)
        {
            // tratando caso de lista com elemento único
            delete ultimo;
            ultimo = primeiro;
            primeiro->prox = nullptr;
        }
        else
        {
            // caso genérico
            No *removido = ultimo;
            ultimo = removido->ant;
            ultimo->prox = nullptr;
            delete removido;
        }
    }

    bool listaVazia()
    {
        // A lista está vazia? true = sim, false = não
        return (primeiro == ultimo);
    }
};

#endif // LISTA_HPP
