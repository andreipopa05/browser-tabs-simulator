/*POPA Andrei-Eduard*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct page {
    int id;
    char url[50];
    char *description;
} page;

typedef struct stack {
    struct stack *next;
    struct page *info;
} stack;

typedef struct tab {
    int id;
    struct page *currentPage;
    struct stack *backwardStack;
    struct stack *forwardStack;
} tab;

typedef struct tabWindow {
    tab *info;
    struct tabWindow *prev;
    struct tabWindow *next;
} tabWindow;

typedef struct tabsList {
    struct tabWindow *sentinel;
} tabsList;

typedef struct browser {
    struct tab *current;
    struct tabsList list;
} browser;

// Functions
browser InitB();
void NEW_TAB(browser *br, int *last_tab);
void CLOSE(browser *br, int *check, int *last_tab);
void OPEN(browser *br, int id, int *check, int *last_tab);
void NEXT(browser *br, int *last_tab);
void PREV(browser *br, int *last_tab);
void PAGE(browser *br, int id, int *check, page *pagesVect, int pag_nr);
void BACKWARD(browser *br, int *check);
void FORWARD(browser *br, int *check);
void PRINT_HISTORY(browser *br, int id, int *check, FILE *fout);
void PRINT(browser *br, FILE *fout);
void FreeB(browser *br);
