/*POPA Andrei-Eduard*/
#include "functions.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input_file> <output_file>\n", argv[0]);
        return 1;
    }

    // Open the input and output files
    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        return 1;
    }
    FILE *fout = fopen(argv[2], "w");
    if (!fout) {
        fclose(file);
        return 1;
    }

    // Allocate memory and read the pages
    int pag_nr = 0;

    fscanf(file, "%d", &pag_nr);

    page *pagesVect = malloc((pag_nr + 1) * sizeof(page));
    if (!pagesVect) {
        return 1;
    }

    char line[256];

    for (int i = 1; i <= pag_nr; i++) {
        fscanf(file, "%d", &pagesVect[i].id);
        fgetc(file); // Skip the newline
        fgets(pagesVect[i].url, sizeof(pagesVect[i].url), file);
        pagesVect[i].url[strcspn(pagesVect[i].url, "\n")] = '\0';
        fgets(line, sizeof(line), file);
        line[strcspn(line, "\n")] = '\0';
        pagesVect[i].description = strdup(line);
    }

    // Read the commands
    int cmd_nr = 0;

    fscanf(file, "%d", &cmd_nr);
    fgetc(file);

    char (*commands)[256] = malloc(cmd_nr * sizeof(*commands));
    if (!commands) {
        return 1;
    }

    for (int i = 0; i < cmd_nr; i++) {
        fgets(commands[i], sizeof(commands[i]), file);
        commands[i][strcspn(commands[i], "\n")] = '\0';
    }

    // Initialize the browser
    browser br = InitB();
    if (!br.list.sentinel || !br.current) {
        return 1;
    }

    // Go through the commands and execute them
    int check = 0;      // Flags whether the command failed
    int last_tab = -1;  // Used to compute the id of the next new tab

    // Buffers for the id argument of OPEN, PAGE and PRINT_HISTORY
    char open_id[4], page_id[4], history_id[4];

    for (int i = 0; i < cmd_nr; i++) {
        check = 0;
        if (strcmp(commands[i], "NEW_TAB") == 0) {
            NEW_TAB((&br), (&last_tab));
        }
        if (strcmp(commands[i], "CLOSE") == 0) {
            CLOSE((&br), (&check), (&last_tab));
            if (check == 1) {
                fprintf(fout, "403 Forbidden\n");
                continue;
            }
        }
        if (sscanf(commands[i], "OPEN %s", open_id) == 1) {
            int open_id_intenger = atoi(open_id);
            OPEN((&br), open_id_intenger, (&check), (&last_tab));
            if (check == 2) {
                fprintf(fout, "403 Forbidden\n");
                continue;
            }
        }
        if (strcmp(commands[i], "NEXT") == 0) {
            NEXT((&br), (&last_tab));
        }
        if (strcmp(commands[i], "PREV") == 0) {
            PREV((&br), (&last_tab));
        }
        if (sscanf(commands[i], "PAGE %s", page_id) == 1) {
            int page_id_intenger = atoi(page_id);
            PAGE((&br), page_id_intenger, (&check), pagesVect, pag_nr);
            if (check == 3) {
                fprintf(fout, "403 Forbidden\n");
                continue;
            }
        }
        if (strcmp(commands[i], "BACKWARD") == 0) {
            BACKWARD((&br), (&check));
            if (check == 5) {
                fprintf(fout, "403 Forbidden\n");
                continue;
            }
        }
        if (strstr(commands[i], "FORWARD") != 0) {
            FORWARD((&br), (&check));
            if (check == 6) {
                fprintf(fout, "403 Forbidden\n");
                continue;
            }
        }
        if (sscanf(commands[i], "PRINT_HISTORY %s", history_id) == 1) {
            int history_id_intenger = atoi(history_id);
            PRINT_HISTORY((&br), history_id_intenger, (&check), fout);
            if (check == 4) {
                fprintf(fout, "403 Forbidden\n");
                continue;
            }
        }
        if (strcmp(commands[i], "PRINT") == 0) {
            PRINT((&br), fout);
        }
    }

    // Free memory and close the files
    for (int i = 1; i <= pag_nr; i++) {
        free(pagesVect[i].description);
    }
    free(pagesVect);
    free(commands);
    FreeB((&br));

    fclose(file);
    fclose(fout);
    return 0;
}
