#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 

// Olá professor! esse trabalho foi feito por mim Hugo Gabriel Alves Franco Moreira - SP 3217566 e com o Lincoln - Sp319986X


typedef struct {
    char assembly_mnemonic[5]; // <-- char de mnemônico
    char opcode_hex[3];       // <-- char de OPCODE
} OpcodeEntry;

// Tabela global de mnemonicos
OpcodeEntry opcode_table[] = {
    {"NOP", "00"},
    {"STA", "10"},
    {"LDA", "20"},
    {"ADD", "30"},
    {"OR", "40"},
    {"AND", "50"},
    {"NOT", "60"},
    {"JMP", "80"},
    {"JN", "90"},
    {"JZ", "A0"},
    {"HLT", "F0"}
};

int num_opcodes = sizeof(opcode_table) / sizeof(OpcodeEntry);

// Função para buscar o opcode hexadecimal dado um mnemônico Assembly
char* get_opcode_hex(char* mnemonic) {
    for (int i = 0; i < num_opcodes; i++) {
        if (strcmp(mnemonic, opcode_table[i].assembly_mnemonic) == 0) {
            return opcode_table[i].opcode_hex;
        }
    }
    return NULL; // caso nao encontre mnemonicos
}


unsigned char hex_to_byte(char* hex_str) {
    unsigned int val;
    sscanf(hex_str, "%x", &val);
    return (unsigned char)val;
}

int main(int argc, char *argv[]) {
    FILE *input_file;
    FILE *output_file;
    char line[256];
    char *token;
    char *opcode_str;
    char *operand_str;
    unsigned char opcode_byte;
    unsigned char operand_byte;


    if (argc != 2) {
        printf("Uso: %s <nome_do_arquivo_assembly.asm>\n", argv[0]);
        return 1;
    }


    input_file = fopen(argv[1], "r");
    if (input_file == NULL) {
        perror("Erro ao abrir o arquivo de entrada");
        return 1;
    }

    char output_filename[260]; // <-- delimitador de 260 caracteres no arquivo asm 
    strcpy(output_filename, argv[1]); 
    
    char *dot = strrchr(output_filename, '.'); // função transformando o arquivo em .mem
    if (dot != NULL) {
        strcpy(dot, ".mem");
    } else { 
        strcat(output_filename, ".mem"); 
    }

    
    output_file = fopen(output_filename, "wb"); // "wb" para escrita binária
    if (output_file == NULL) {
        perror("Erro ao criar o arquivo de saída");
        fclose(input_file);
        return 1;
    }

   
    unsigned char header[] = {0x03, 0x4E, 0x44, 0x52};
    fwrite(header, sizeof(unsigned char), 4, output_file);

    // Loop de leitura do arquivo .asm e tradução
    while (fgets(line, sizeof(line), input_file) != NULL) {
       
        line[strcspn(line, "\n\r")] = 0;
     
        // Ex: "1 LDA 80" -> "1", "LDA", "80"
        token = strtok(line, " \t"); // Pega o número da linha (pode ser ignorado)
        if (token == NULL) continue; // linha vazia ou com apenas espaços

        token = strtok(NULL, " \t"); // Pega o mnemonico (ex: "LDA")
        if (token == NULL) continue; // Mnemonico ausente

        opcode_str = get_opcode_hex(token); // Busca o opcode em hexadecimal
        if (opcode_str == NULL) {
            fprintf(stderr, "Erro: Mnemônico desconhecido: %s na linha %s\n", token, line); // 
            
            continue;
        }

        opcode_byte = hex_to_byte(opcode_str);

        // Escrever o Opcode 
        fwrite(&opcode_byte, sizeof(unsigned char), 1, output_file);
        // Escrever o byte adicional 00 
        unsigned char zero_byte = 0x00;
        fwrite(&zero_byte, sizeof(unsigned char), 1, output_file);

        // Verificar se a instrução possui operando (endereço)
        // Instruções que NÃO possuem operando: NOT, NOP, HLT 
        if (strcmp(token, "NOT") != 0 && strcmp(token, "NOP") != 0 && strcmp(token, "HLT") != 0) {
            operand_str = strtok(NULL, " \t"); // Pega o operando (ex: "80")
            if (operand_str == NULL) {
                fprintf(stderr, "Erro: Operando esperado para a instrução %s na linha %s\n", token, line);
                continue;
            }
            operand_byte = hex_to_byte(operand_str);

            // Escrever o operando 
            fwrite(&operand_byte, sizeof(unsigned char), 1, output_file);
            // Escrever o byte adicional 00 
            fwrite(&zero_byte, sizeof(unsigned char), 1, output_file);
        }
    }

    // 8. Fechar os arquivos
    fclose(input_file);
    fclose(output_file);

    printf("Arquivo %s traduzido para %s com sucesso!\n", argv[1], output_filename);

    return 0;
}