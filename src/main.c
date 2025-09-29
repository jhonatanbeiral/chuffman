#include <stdio.h>
#include <string.h>
#include "huffman.h"

#define MAX_PATH 256

int main(void) {
    char opcao[10];
    char inpath[MAX_PATH];
    char outpath[MAX_PATH];

    printf("=====================================\n");
    printf("     COMPRESSOR HUFFMAN EM C\n");
    printf("=====================================\n");
    printf("Escolha uma opção:\n");
    printf("  [c] - Comprimir arquivo\n");
    printf("  [d] - Descomprimir arquivo\n");
    printf("Opção: ");

    if (!fgets(opcao, sizeof(opcao), stdin)) {
        fprintf(stderr, "Erro ao ler opção.\n");
        return 1;
    }

    // remove o '\n' do fgets
    opcao[strcspn(opcao, "\n")] = '\0';

    if (strcmp(opcao, "c") == 0) {
        printf("\nDigite o nome do arquivo de entrada: ");
        if (!fgets(inpath, sizeof(inpath), stdin)) return 1;
        inpath[strcspn(inpath, "\n")] = '\0';

        printf("Digite o nome do arquivo de saída (.huf): ");
        if (!fgets(outpath, sizeof(outpath), stdin)) return 1;
        outpath[strcspn(outpath, "\n")] = '\0';

        if (compress_file(inpath, outpath) == 0) {
            printf("Arquivo '%s' comprimido com sucesso em '%s'.\n", inpath, outpath);
        } else {
            fprintf(stderr, "Erro ao comprimir '%s'.\n", inpath);
            return 2;
        }

    } else if (strcmp(opcao, "d") == 0) {
        printf("\nDigite o nome do arquivo comprimido (.huf): ");
        if (!fgets(inpath, sizeof(inpath), stdin)) return 1;
        inpath[strcspn(inpath, "\n")] = '\0';

        printf("Digite o nome do arquivo de saída (texto recuperado): ");
        if (!fgets(outpath, sizeof(outpath), stdin)) return 1;
        outpath[strcspn(outpath, "\n")] = '\0';

        if (decompress_file(inpath, outpath) == 0) {
            printf("Arquivo '%s' descomprimido com sucesso em '%s'.\n", inpath, outpath);
        } else {
            fprintf(stderr, "Erro ao descomprimir '%s'.\n", inpath);
            return 3;
        }

    } else {
        printf("Opção inválida! Use 'c' ou 'd'.\n");
        return 1;
    }

    printf("=====================================\n");
    printf("        OPERAÇÃO FINALIZADA\n");
    printf("=====================================\n");
    return 0;
}

