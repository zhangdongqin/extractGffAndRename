#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 4096
#define MAX_GENE_NAME_LENGTH 128

typedef struct {
    char old_name[MAX_GENE_NAME_LENGTH];
    char new_name[MAX_GENE_NAME_LENGTH];
} GeneNamePair;

void extract_and_rename_gff(const char *gff_in, const char *gff_out, const char *replace_file) {
    GeneNamePair replacements[MAX_LINE_LENGTH];
    int num_replacements = 0;

    // Read the replace file and store it in the replacements array
    FILE *replace_f = fopen(replace_file, "r");
    if (replace_f == NULL) {
        perror("Error opening replace file");
        exit(EXIT_FAILURE);
    }

    while (fscanf(replace_f, "%s\t%s", replacements[num_replacements].old_name, replacements[num_replacements].new_name) == 2) {
        num_replacements++;
    }

    fclose(replace_f);

    // Extract annotations from the input GFF file and rename genes
    FILE *in_f = fopen(gff_in, "r");
    if (in_f == NULL) {
        perror("Error opening input GFF file");
        exit(EXIT_FAILURE);
    }

    FILE *out_f = fopen(gff_out, "w");
    if (out_f == NULL) {
        perror("Error opening output GFF file");
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, MAX_LINE_LENGTH, in_f)) {
        if (line[0] == '#') {
            continue;
        }

        // Parse GFF line and extract gene name
        char *fields[9];
        fields[0] = strtok(line, "\t");
        for (int i = 1; i < 9; i++) {
            fields[i] = strtok(NULL, "\t");
        }
        char *attr_str = fields[8];

        // Check if the gene needs to be extracted
        int gene_index = -1;
        for (int i = 0; i < num_replacements; i++) {
            if (strstr(attr_str, replacements[i].old_name) != NULL) {
                gene_index = i;
                break;
            }
        }

        if (gene_index != -1) {
            // Rename gene
            char new_attr_str[MAX_LINE_LENGTH];
            strcpy(new_attr_str, attr_str);
            for (int i = 0; i < num_replacements; i++) {
                char old_name_with_eq[MAX_GENE_NAME_LENGTH + 1];
                sprintf(old_name_with_eq, "=%s", replacements[i].old_name);
                char *ptr = strstr(new_attr_str, old_name_with_eq);
                if (ptr != NULL) {
                    char new_name_with_eq[MAX_GENE_NAME_LENGTH + 1];
                    sprintf(new_name_with_eq, "=%s", replacements[i].new_name);
                    strncpy(ptr, new_name_with_eq, strlen(new_name_with_eq));
                }
            }

            // Write the modified line to the output file
            fields[8] = new_attr_str;
            for (int i = 0; i < 8; i++) {
                fprintf(out_f, "%s\t", fields[i]);
            }
            fprintf(out_f, "%s", fields[8]);
        }
    }

    fclose(in_f);
    fclose(out_f);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <gff_in> <gff_out> <replace_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *gff_in = argv[1];
    const char *gff_out = argv[2];
    const char *replace_file = argv[3];

    extract_and_rename_gff(gff_in, gff_out, replace_file);

    return 0;
}
