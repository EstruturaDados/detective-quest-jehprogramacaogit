#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM_HASH 10
#define TAM_MAX 100

// ----------- Structs -----------

typedef struct Sala {
    char nome[50];
    struct Sala* esquerda;
    struct Sala* direita;
} Sala;

typedef struct Pista {
    char descricao[100];
    struct Pista* esquerda;
    struct Pista* direita;
} Pista;

typedef struct HashNode {
    char pista[100];
    char suspeito[50];
    struct HashNode* proximo;
} HashNode;

// ----------- Funções -----------

// Cria dinamicamente um cômodo.
Sala* criarSala(char* nome) {
    Sala* novaSala = (Sala*) malloc(sizeof(Sala));
    strcpy(novaSala->nome, nome);
    novaSala->esquerda = NULL;
    novaSala->direita = NULL;
    return novaSala;
}

// Retorna a pista associada a uma sala específica.
char* obterPista(char* nomeSala) {
    if (strcmp(nomeSala, "Biblioteca") == 0)
        return "Livro rasgado";
    else if (strcmp(nomeSala, "Cozinha") == 0)
        return "Faca suja";
    else if (strcmp(nomeSala, "Sala de Estar") == 0)
        return "Pegada de lama";
    else if (strcmp(nomeSala, "Quarto") == 0)
        return "Perfume caro";
    else if (strcmp(nomeSala, "Banheiro") == 0)
        return "Toalha molhada";
    return NULL;
}

// Insere uma pista coletada na árvore BST.
Pista* inserirPista(Pista* raiz, char* descricao) {
    if (raiz == NULL) {
        Pista* nova = (Pista*) malloc(sizeof(Pista));
        strcpy(nova->descricao, descricao);
        nova->esquerda = nova->direita = NULL;
        return nova;
    }
    if (strcmp(descricao, raiz->descricao) < 0)
        raiz->esquerda = inserirPista(raiz->esquerda, descricao);
    else if (strcmp(descricao, raiz->descricao) > 0)
        raiz->direita = inserirPista(raiz->direita, descricao);
    return raiz;
}

// Função hash simples para mapear strings.
int hash(char* str) {
    int soma = 0;
    for (int i = 0; str[i]; i++) {
        soma += str[i];
    }
    return soma % TAM_HASH;
}

// Insere associação pista/suspeito na tabela hash.
void inserirNaHash(HashNode* tabela[], char* pista, char* suspeito) {
    int indice = hash(pista);
    HashNode* novo = (HashNode*) malloc(sizeof(HashNode));
    strcpy(novo->pista, pista);
    strcpy(novo->suspeito, suspeito);
    novo->proximo = tabela[indice];
    tabela[indice] = novo;
}

// Consulta o suspeito correspondente a uma pista.
char* encontrarSuspeito(HashNode* tabela[], char* pista) {
    int indice = hash(pista);
    HashNode* atual = tabela[indice];
    while (atual != NULL) {
        if (strcmp(atual->pista, pista) == 0) {
            return atual->suspeito;
        }
        atual = atual->proximo;
    }
    return NULL;
}

// Lista todas as pistas da BST.
void listarPistas(Pista* raiz) {
    if (raiz != NULL) {
        listarPistas(raiz->esquerda);
        printf("- %s\n", raiz->descricao);
        listarPistas(raiz->direita);
    }
}

// Conta quantas pistas apontam para determinado suspeito.
int contarPistasDoSuspeito(Pista* raiz, HashNode* tabela[], char* suspeito) {
    if (raiz == NULL) return 0;
    int count = 0;
    char* suspeitoAtual = encontrarSuspeito(tabela, raiz->descricao);
    if (suspeitoAtual != NULL && strcmp(suspeitoAtual, suspeito) == 0)
        count++;
    count += contarPistasDoSuspeito(raiz->esquerda, tabela, suspeito);
    count += contarPistasDoSuspeito(raiz->direita, tabela, suspeito);
    return count;
}

// Navega pela árvore e ativa o sistema de pistas.
void explorarSalas(Sala* atual, Pista** arvorePistas, HashNode* hashTable[]) {
    char opcao;
    while (1) {
        printf("\nVocê está na sala: %s\n", atual->nome);
        char* pista = obterPista(atual->nome);
        if (pista != NULL) {
            printf("Você encontrou uma pista: %s\n", pista);
            *arvorePistas = inserirPista(*arvorePistas, pista);
        }

        printf("Para onde deseja ir? (e = esquerda, d = direita, s = sair): ");
        scanf(" %c", &opcao);
        if (opcao == 's') break;
        else if (opcao == 'e' && atual->esquerda != NULL) atual = atual->esquerda;
        else if (opcao == 'd' && atual->direita != NULL) atual = atual->direita;
        else printf("Direção inválida ou caminho inexistente!\n");
    }
}

// Conduz à fase de julgamento final.
void verificarSuspeitoFinal(Pista* arvorePistas, HashNode* tabela[]) {
    char suspeito[50];
    printf("\n--- JULGAMENTO FINAL ---\n");
    printf("Pistas coletadas:\n");
    listarPistas(arvorePistas);

    printf("\nQuem você acha que é o culpado? ");
    scanf(" %[^\n]", suspeito);

    int total = contarPistasDoSuspeito(arvorePistas, tabela, suspeito);
    if (total >= 2) {
        printf("\n🔍 Parabéns! Você acertou. %d pistas apontam para %s.\n", total, suspeito);
    } else {
        printf("\n❌ Infelizmente, não há evidências suficientes contra %s.\n", suspeito);
    }
}

// ----------- Main -----------

int main() {
    // Monta a árvore binária dos cômodos
    Sala* salaEstar = criarSala("Sala de Estar");
    salaEstar->esquerda = criarSala("Biblioteca");
    salaEstar->direita = criarSala("Cozinha");
    salaEstar->esquerda->esquerda = criarSala("Quarto");
    salaEstar->direita->direita = criarSala("Banheiro");

    // Inicializa a tabela hash
    HashNode* hashTable[TAM_HASH] = {NULL};
    inserirNaHash(hashTable, "Livro rasgado", "Sr. Mustard");
    inserirNaHash(hashTable, "Faca suja", "Cozinheira");
    inserirNaHash(hashTable, "Pegada de lama", "Jardineiro");
    inserirNaHash(hashTable, "Perfume caro", "Madame Rose");
    inserirNaHash(hashTable, "Toalha molhada", "Sr. Mustard");

    // Inicializa a árvore de pistas vazia
    Pista* arvorePistas = NULL;

    // Introdução
    printf("==================================\n");
    printf("       🕵️  DETECTIVE QUEST       \n");
    printf("==================================\n");
    printf("Explore a mansão e descubra quem é o culpado!\n");

    // Inicia exploração
    explorarSalas(salaEstar, &arvorePistas, hashTable);

    // Julgamento final
    verificarSuspeitoFinal(arvorePistas, hashTable);

    printf("\nObrigado por jogar!\n");
    return 0;
}