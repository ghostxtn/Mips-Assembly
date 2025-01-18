#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 256
#define START_ADDRESS 0x00400000

const char *opcodes[] = {
    "000000", // add
    "000000", // sub
    "011100", // mul
    "000000", // and
    "000000"  // xor
};

const char *funct_codes[] = {
    "100000", // add
    "100010", // sub
    "000010", // mul
    "100100", // and
    "100110"  // xor
};


char **read_input_file(const char *filename, int *line_count) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char **lines = malloc(sizeof(char *) * MAX_LINE_LENGTH);
    *line_count = 0;

    char buffer[MAX_LINE_LENGTH];
    while (fgets(buffer, MAX_LINE_LENGTH, file)) {
        lines[*line_count] = strdup(buffer);
        (*line_count)++;
    }

    fclose(file);
    return lines;
}


void parse_instruction(const char *line, char *operation, char *registers[]) {
    char temp[MAX_LINE_LENGTH];
    strcpy(temp, line);

    char *token = strtok(temp, " ");
    strcpy(operation, token);

    token = strtok(NULL, ",");
    for (int i = 0; i < 3; i++) {
        registers[i] = strdup(token);
        token = strtok(NULL, ",");
    }
}


void register_to_binary(const char *reg, char *binary) {
    int reg_num = atoi(reg + 1);
    snprintf(binary, 6, "%05d", reg_num);
}


void translate_instruction(const char *operation, char *registers[], char *machine_code) {
    int index = 0;
    if (strcmp(operation, "add") == 0) index = 0;
    else if (strcmp(operation, "sub") == 0) index = 1;
    else if (strcmp(operation, "mul") == 0) index = 2;
    else if (strcmp(operation, "and") == 0) index = 3;
    else if (strcmp(operation, "xor") == 0) index = 4;

    char rs[6], rt[6], rd[6];
    register_to_binary(registers[1], rs);
    register_to_binary(registers[2], rt);
    register_to_binary(registers[0], rd);

    snprintf(machine_code, 33, "%s%s%s%s00000%s", opcodes[index], rs, rt, rd, funct_codes[index]);
}


void write_output_file(const char *filename, char **instructions, int count) {
    char output_filename[MAX_LINE_LENGTH];
    strcpy(output_filename, filename);
    char *ext = strrchr(output_filename, '.');
    if (ext) strcpy(ext, ".obj");
    else strcat(output_filename, ".obj");

    FILE *file = fopen(output_filename, "w");
    if (!file) {
        perror("Error opening output file");
        exit(EXIT_FAILURE);
    }

    int address = START_ADDRESS;
    for (int i = 0; i < count; i++) {
        fprintf(file, "0x%08X: %s\n", address, instructions[i]);
        address += 4;
    }

    fclose(file);
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s mycode.asm \n", argv[0]);
        return EXIT_FAILURE;
    }

    printf("Input file: %s\n", argv[1]); // Debugging output

    int line_count;
    char **lines = read_input_file(argv[1], &line_count);

    if (line_count == 0) {
        fprintf(stderr, "Input file is empty or invalid.\n");
        return EXIT_FAILURE;
    }

    char **instructions = malloc(sizeof(char *) * line_count);

    for (int i = 0; i < line_count; i++) {
        char operation[10];
        char *registers[3];
        char machine_code[33];

        parse_instruction(lines[i], operation, registers);
        translate_instruction(operation, registers, machine_code);
        instructions[i] = strdup(machine_code);

        for (int j = 0; j < 3; j++) free(registers[j]);
    }

    write_output_file(argv[1], instructions, line_count);

    printf("Output file generated successfully.\n");

    for (int i = 0; i < line_count; i++) {
        free(lines[i]);
        free(instructions[i]);
    }
    free(lines);
    free(instructions);

    return EXIT_SUCCESS;
}
