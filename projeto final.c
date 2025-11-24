/*
 * calc_notas.c
 * Calculadora de Estatísticas de Notas
 *
 * Funcionalidades:
 *  - Inserir várias notas (validadas: 0.0 a 10.0)
 *  - Armazenamento dinâmico em vetor (realloc)
 *  - Mostrar lista de notas
 *  - Calcular média, maior, menor, desvio padrão
 *  - Ordenar notas (ascendente)
 *  - Salvar/Carregar notas em arquivo texto
 *  - Menu interativo
 *
 * Compilar:
 *   gcc -std=c11 -Wall -O2 -o calc_notas calc_notas.c -lm
 *
 * Executar:
 *   ./calc_notas
 *
 * Autor: Implementação conforme especificação do usuário
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define INICIAL_CAPACIDADE 10
#define BUFFER_TAMANHO 256

/* --- Prototipos --- */
void imprimir_menu(void);
void adicionar_notas(double **vetor, size_t *n, size_t *capacidade);
void mostrar_notas(const double *vetor, size_t n);
double calcular_media(const double *vetor, size_t n);
double maior_nota(const double *vetor, size_t n);
double menor_nota(const double *vetor, size_t n);
double desvio_padrao(const double *vetor, size_t n);
void ordenar_notas(double *vetor, size_t n);
int salvar_em_arquivo(const char *nome, const double *vetor, size_t n);
int carregar_de_arquivo(const char *nome, double **vetor, size_t *n, size_t *capacidade);
void limpar_buffer(void);

/* --- Função principal --- */
int main(void) {
    double *notas = NULL;
    size_t n = 0;
    size_t capacidade = 0;
    int opcao = 0;
    char filename[BUFFER_TAMANHO];

    /* inicializa vetor */
    capacidade = INICIAL_CAPACIDADE;
    notas = malloc(capacidade * sizeof(double));
    if (!notas) {
        fprintf(stderr, "Erro: Falha ao alocar memória.\n");
        return 1;
    }

    do {
        imprimir_menu();
        printf("Escolha uma opcao: ");
        if (scanf("%d", &opcao) != 1) {
            printf("Entrada invalida. Tente novamente.\n");
            limpar_buffer();
            continue;
        }

        switch (opcao) {
            case 1:
                adicionar_notas(&notas, &n, &capacidade);
                break;
            case 2:
                mostrar_notas(notas, n);
                break;
            case 3:
                if (n == 0) {
                    printf("Nenhuma nota cadastrada.\n");
                } else {
                    printf("Media: %.2f\n", calcular_media(notas, n));
                    printf("Maior nota: %.2f\n", maior_nota(notas, n));
                    printf("Menor nota: %.2f\n", menor_nota(notas, n));
                    printf("Desvio padrao: %.2f\n", desvio_padrao(notas, n));
                }
                break;
            case 4:
                if (n == 0) {
                    printf("Nenhuma nota para ordenar.\n");
                } else {
                    ordenar_notas(notas, n);
                    printf("Notas ordenadas em ordem crescente.\n");
                }
                break;
            case 5:
                printf("Nome do arquivo para salvar (ex: notas.txt): ");
                scanf("%s", filename);
                if (salvar_em_arquivo(filename, notas, n) == 0)
                    printf("Salvo com sucesso em '%s'.\n", filename);
                else
                    printf("Erro ao salvar em arquivo.\n");
                break;
            case 6:
                printf("Nome do arquivo para carregar (ex: notas.txt): ");
                scanf("%s", filename);
                if (carregar_de_arquivo(filename, &notas, &n, &capacidade) == 0)
                    printf("Carregado com sucesso de '%s'. Total de notas: %zu\n", filename, n);
                else
                    printf("Erro ao carregar arquivo.\n");
                break;
            case 7:
                printf("Saindo. Ate logo!\n");
                break;
            default:
                printf("Opcao invalida. Tente novamente.\n");
        }

    } while (opcao != 7);

    free(notas);
    return 0;
}

/* --- Implementações --- */

void imprimir_menu(void) {
    printf("\n=========================================\n");
    printf(" Calculadora de Estatisticas de Notas\n");
    printf("=========================================\n");
    printf("1. Inserir/Adicionar notas\n");
    printf("2. Mostrar todas as notas\n");
    printf("3. Calcular estatisticas (media, maior, menor, desvio padrao)\n");
    printf("4. Ordenar notas (crescente)\n");
    printf("5. Salvar notas em arquivo\n");
    printf("6. Carregar notas de arquivo\n");
    printf("7. Sair\n");
    printf("=========================================\n");
}

void adicionar_notas(double **vetor, size_t *n, size_t *capacidade) {
    double valor;
    char resposta[BUFFER_TAMANHO];

    /* ler em loop até o usuário decidir parar */
    while (1) {
        printf("Digite uma nota (0.0 - 10.0) ou 's' para parar: ");
        if (scanf("%s", resposta) != 1) {
            printf("Entrada invalida.\n");
            limpar_buffer();
            continue;
        }

        if (strcmp(resposta, "s") == 0 || strcmp(resposta, "S") == 0) {
            break;
        }

        /* tenta converter para double */
        char *endptr;
        valor = strtod(resposta, &endptr);
        if (endptr == resposta || *endptr != '\0') {
            printf("Entrada invalida. Digite um numero ou 's' para sair.\n");
            continue;
        }

        /* validação */
        if (valor < 0.0 || valor > 10.0) {
            printf("Nota invalida. Deve estar entre 0.0 e 10.0.\n");
            continue;
        }

        /* realoca se necessario */
        if (*n >= *capacidade) {
            size_t nova_cap = (*capacidade) * 2;
            double *tmp = realloc(*vetor, nova_cap * sizeof(double));
            if (!tmp) {
                fprintf(stderr, "Erro: Falha ao realocar memoria.\n");
                return;
            }
            *vetor = tmp;
            *capacidade = nova_cap;
        }

        /* adiciona nota */
        (*vetor)[*n] = valor;
        (*n)++;
        printf("Nota %.2f adicionada. Total: %zu\n", valor, *n);
    }
}

void mostrar_notas(const double *vetor, size_t n) {
    if (n == 0) {
        printf("Nenhuma nota cadastrada.\n");
        return;
    }
    printf("Lista de notas (total %zu):\n", n);
    for (size_t i = 0; i < n; ++i) {
        printf("%3zu: %.2f\n", i+1, vetor[i]);
    }
}

double calcular_media(const double *vetor, size_t n) {
    if (n == 0) return 0.0;
    double soma = 0.0;
    for (size_t i = 0; i < n; ++i) {
        soma += vetor[i];
    }
    return soma / (double)n;
}

double maior_nota(const double *vetor, size_t n) {
    if (n == 0) return 0.0;
    double maior = vetor[0];
    for (size_t i = 1; i < n; ++i) {
        if (vetor[i] > maior) maior = vetor[i];
    }
    return maior;
}

double menor_nota(const double *vetor, size_t n) {
    if (n == 0) return 0.0;
    double menor = vetor[0];
    for (size_t i = 1; i < n; ++i) {
        if (vetor[i] < menor) menor = vetor[i];
    }
    return menor;
}

double desvio_padrao(const double *vetor, size_t n) {
    if (n == 0) return 0.0;
    double media = calcular_media(vetor, n);
    double soma_quad = 0.0;
    for (size_t i = 0; i < n; ++i) {
        double diff = vetor[i] - media;
        soma_quad += diff * diff;
    }
    /* Desvio padrao populacional (dividir por n). 
       Se preferir amostral, dividir por (n-1) quando n>1. */
    return sqrt(soma_quad / (double)n);
}

/* Simple insertion sort (suficiente para tamanhos pequenos a médios) */
void ordenar_notas(double *vetor, size_t n) {
    for (size_t i = 1; i < n; ++i) {
        double chave = vetor[i];
        ssize_t j = (ssize_t)i - 1;
        while (j >= 0 && vetor[j] > chave) {
            vetor[j+1] = vetor[j];
            j--;
        }
        vetor[j+1] = chave;
    }
}

/* Salva notas em arquivo texto, 1 nota por linha */
int salvar_em_arquivo(const char *nome, const double *vetor, size_t n) {
    FILE *f = fopen(nome, "w");
    if (!f) return -1;
    for (size_t i = 0; i < n; ++i) {
        if (fprintf(f, "%.10g\n", vetor[i]) < 0) {
            fclose(f);
            return -1;
        }
    }
    fclose(f);
    return 0;
}

/* Carrega notas de arquivo texto (1 nota por linha). Recria o vetor. */
int carregar_de_arquivo(const char *nome, double **vetor, size_t *n, size_t *capacidade) {
    FILE *f = fopen(nome, "r");
    if (!f) return -1;

    /* liberar vetor atual e criar novo */
    free(*vetor);
    *vetor = NULL;
    *n = 0;
    *capacidade = INICIAL_CAPACIDADE;
    *vetor = malloc((*capacidade) * sizeof(double));
    if (!*vetor) {
        fclose(f);
        return -1;
    }

    char linha[BUFFER_TAMANHO];
    while (fgets(linha, sizeof(linha), f)) {
        char *endptr;
        double val = strtod(linha, &endptr);
        if (endptr == linha) {
            /* linha nao contem numero - ignora */
            continue;
        }
        /* valida nota */
        if (val < 0.0 || val > 10.0) {
            /* ignora notas invalidas no arquivo */
            continue;
        }

        if (*n >= *capacidade) {
            size_t nova = (*capacidade) * 2;
            double *tmp = realloc(*vetor, nova * sizeof(double));
            if (!tmp) {
                fclose(f);
                return -1;
            }
            *vetor = tmp;
            *capacidade = nova;
        }

        (*vetor)[*n] = val;
        (*n)++;
    }

    fclose(f);
    return 0;
}

/* Limpa resto do buffer stdin */
void limpar_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

