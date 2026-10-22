/*POPA Andrei-Eduard*/
#include "functions.h"

browser InitB() { // Initializes the browser: creates tab 0 with page 0
    browser br;

    // Allocate memory
    br.current = (tab*)malloc(sizeof(tab));
    if (!br.current) {
        return br;
    }
    br.current->currentPage = (page*)malloc(sizeof(page));
    if (!br.current->currentPage) {
        free(br.current);
        return br;
    }
    tabWindow *aux = (tabWindow*)malloc(sizeof(tabWindow));
    if (!aux) {
        free(br.current->currentPage);
        free(br.current);
        return br;
    }
    aux->info = (tab*)malloc(sizeof(tab));
    if (!aux->info) {
        free(br.current->currentPage);
        free(br.current);
        free(aux);
        return br;
    }
    aux->info->currentPage = (page*)malloc(sizeof(page));
    if (!aux->info->currentPage) {
        free(br.current->currentPage);
        free(br.current);
        free(aux->info);
        free(aux);
        return br;
    }
    br.list.sentinel = (tabWindow *)malloc(sizeof(tabWindow));
    if (!br.list.sentinel) {
        free(br.current->currentPage);
        free(br.current);
        free(aux->info->currentPage);
        free(aux->info);
        free(aux);
        return br;
    }
    br.list.sentinel->info = NULL;

    // Initialize TAB 0
    aux->info->id = 0;
    aux->info->backwardStack = NULL;
    aux->info->forwardStack = NULL;

    // Initialize PAGE 0
    aux->info->currentPage->id = 0;
    strcpy(aux->info->currentPage->url, "https://acs.pub.ro/");
    aux->info->currentPage->description = strdup("Computer Science");

    // Free the initial allocations, replaced by tab 0
    free(br.current->currentPage);
    free(br.current);

    br.current = aux->info;

    // Link tab 0 to the sentinel
    br.list.sentinel->next = aux;
    br.list.sentinel->prev = aux;
    aux->next = br.list.sentinel;
    aux->prev = br.list.sentinel;

    return br;
}

void NEW_TAB(browser *br, int *last_tab) {
    if (!br || !br->list.sentinel || !br->current) {
        return;
    }
    tabWindow *tb = (tabWindow*)malloc(sizeof(tabWindow));
    if (!tb) {
        return;
    }
    tb->info = (tab*)malloc(sizeof(tab));
    if (!tb->info) {
        free(tb);
        return;
    }
    tb->info->currentPage = (page*)malloc(sizeof(page));
    if (!tb->info->currentPage) {
        free(tb->info);
        free(tb);
        return;
    }

    // Initialize the new tab

    // Compute the new tab's id based on earlier changes to the list
    if (*last_tab == -1) {
        tb->info->id = br->current->id + 1;
    } else {
        tb->info->id = *last_tab + 1;
        *last_tab = -1;
    }

    tb->info->currentPage->id = 0;
    strcpy(tb->info->currentPage->url, "https://acs.pub.ro/");
    tb->info->currentPage->description = strdup("Computer Science");
    tb->info->backwardStack = NULL;
    tb->info->forwardStack = NULL;

    // Set the new tab's next/prev pointers
    tb->next = br->list.sentinel;
    tb->prev = br->list.sentinel->prev;

    // Link the new tab into the list, before the sentinel
    br->list.sentinel->prev->next = tb;
    br->list.sentinel->prev = tb;

    // Switch the current tab
    br->current = tb->info;
}

// Frees a stack of pages
static void FreeStack(stack **top) {
    while (*top) {
        stack *aux = *top;
        *top = (*top)->next;
        if (aux->info) {
            free(aux->info->description);
            free(aux->info);
        }
        free(aux);
    }
}

void CLOSE(browser *br, int *check, int *last_tab) {
    if (!br || !br->list.sentinel || !br->current) {
        return;
    }
    if (br->current->id == 0) {
        *check = 1;
        return;
    }

    // Save the id of the last tab in the list
    if (*last_tab == -1) {
        *last_tab = br->list.sentinel->prev->info->id;
    }

    // Removing the last tab
    if (br->current == br->list.sentinel->prev->info) {
        tabWindow *aux = br->list.sentinel->prev;

        // Fix the links
        aux->prev->next = br->list.sentinel;
        br->list.sentinel->prev = aux->prev;

        // Free the closed tab
        FreeStack(&aux->info->forwardStack);
        FreeStack(&aux->info->backwardStack);
        free(aux->info->currentPage->description);
        free(aux->info->currentPage);
        free(aux->info);
        free(aux);

        // Change the current tab
        br->current = br->list.sentinel->prev->info;
    } else {  // The removed tab is not the last one
        tabWindow *aux = br->list.sentinel->next;

        while (aux->info != br->current) {
            aux = aux->next;
        }

        // Fix the links
        aux->prev->next = aux->next;
        aux->next->prev = aux->prev;

        // Save the tab that will become current
        tabWindow *ant = aux->prev;

        // Free the closed tab
        aux->prev = NULL;
        aux->next = NULL;
        FreeStack(&aux->info->forwardStack);
        FreeStack(&aux->info->backwardStack);
        free(aux->info->currentPage->description);
        free(aux->info->currentPage);
        free(aux->info);
        free(aux);

        // Change the current tab
        br->current = ant->info;
    }
}

void OPEN(browser *br, int id, int *check, int *last_tab) {
    if (!br || !br->list.sentinel || !br->current) {
        return;
    }
    if (id > br->list.sentinel->prev->info->id) {  // Error check
        *check = 2;
        return;
    }

    tabWindow *aux = br->list.sentinel->next;

    // Search for the requested tab
    while (aux != br->list.sentinel && aux->info->id != id) {
        aux = aux->next;
    }

     if (aux == br->list.sentinel) {  // tab was closed earlier => error
        *check = 2;
        return;
    }

    // Save the id of the last tab in the list
    if (*last_tab == -1) {
        *last_tab = br->list.sentinel->prev->info->id;
    }

    // Make it the current tab
    br->current = aux->info;
}

void NEXT(browser *br, int *last_tab) {
    if (!br || !br->list.sentinel || !br->current) {
        return;
    }
    if (br->current->id == br->list.sentinel->prev->info->id) {
        // Save the id of the last tab in the list (in both cases)
        if (*last_tab == -1) {
            *last_tab = br->list.sentinel->prev->info->id;
        }

        br->current = br->list.sentinel->next->info;
        return;
    }

    tabWindow *aux = br->list.sentinel->next;

    // Save the id of the last tab in the list
    if (*last_tab == -1) {
        *last_tab = br->list.sentinel->prev->info->id;
    }

    // Find the current tab, then move to the next one
    while (aux->info->id != br->current->id) {
        aux = aux->next;
    }
    aux = aux->next;

    br->current = aux->info;
}

void PREV(browser *br, int *last_tab) {
    if (!br || !br->current || !br->list.sentinel) {
        return;
    }
    // Check whether the previous tab is the sentinel (current is the first tab)
    if (br->current->id == br->list.sentinel->next->info->id) {
        br->current = br->list.sentinel->prev->info;
        return;
    }

    tabWindow *aux = br->list.sentinel->next;

    // Save the id of the last tab in the list
    if (*last_tab == -1) {
        *last_tab = br->list.sentinel->prev->info->id;
    }

    // Find the current tab, then move to the previous one
    while (aux->info->id != br->current->id) {
        aux = aux->next;
    }

    br->current = aux->prev->info;
}

void PAGE(browser *br, int id, int *check, page *pagesVect, int pag_nr) {
    if (!br || !br->current || !br->list.sentinel) {
        return;
    }

    *check = 3;
    int searched_page = 0;  // Save the index of the page to add
    for (int i = 1; i <= pag_nr; i++) {  // Look for the requested page
        if (pagesVect[i].id == id) {
            searched_page = i;
            *check = 0;
            break;
        }
    }
    if (*check == 3) {  // Error check
        return;
    }

    stack *new_page = (stack*)malloc(sizeof(stack));
    if (!new_page) {
        return;
    }

    tab *aux = br->current;

    // Push the current page onto the backward stack
    new_page->info = aux->currentPage;
    new_page->next = aux->backwardStack;
    aux->backwardStack = new_page;

    // Set the requested page as the current page
    aux->currentPage = malloc(sizeof(page));
    *aux->currentPage = pagesVect[searched_page];
    aux->currentPage->description =
    strdup(pagesVect[searched_page].description);

    // Free the forward stack
    FreeStack(&aux->forwardStack);
}

void BACKWARD(browser *br, int *check) {
    if (!br || !br->current) {
        return;
    }
    if (br->current->backwardStack == NULL) {  // Error check (empty backward history)
        *check = 5;
        return;
    }

    stack *page_forward = (stack*)malloc(sizeof(stack));
    if (!page_forward) {
        return;
    }

    tab *tb = br->current;

    // Push the current page onto the forward stack
    page_forward->info = tb->currentPage;
    page_forward->next = tb->forwardStack;
    tb->forwardStack = page_forward;

    // The top page of the backward stack becomes the current page
    tb->currentPage = tb->backwardStack->info;

    // Remove the popped node from the stack
    stack *aux = br->current->backwardStack;
    tb->backwardStack = aux->next;
    free(aux);
}

void FORWARD(browser *br, int *check) {
    if (!br || !br->current) {
        return;
    }
    if (br->current->forwardStack == NULL) {
        *check = 6;
        return;
    }

    stack *page_backward = (stack*)malloc(sizeof(stack));
    if (!page_backward) {
        return;
    }

    tab *tb = br->current;

    // ush the current page onto the backward stack
    page_backward->info = tb->currentPage;
    page_backward->next = tb->backwardStack;
    tb->backwardStack = page_backward;

    // The top page of the forward stack becomes the current page
    stack *page_forward = tb->forwardStack;
    tb->currentPage = tb->forwardStack->info;

    // Remove the popped node from the stack
    tb->forwardStack = tb->forwardStack->next;
    free(page_forward);
}

void PRINT_HISTORY(browser *br, int id, int *check, FILE *fout) {
    if (!br || !br->current || !br->list.sentinel) {
        return;
    }
    if (br->list.sentinel->prev->info->id < id) {
        *check = 4;
        return;
    }

    tabWindow *aux = br->list.sentinel->next;
    while (aux != br->list.sentinel && aux->info->id != id) {
        aux = aux->next;
    }

    if (aux == br->list.sentinel) {
        *check = 4;
        return;
    }

    tab *tb = aux->info;
    stack *stack_pass_f = tb->forwardStack;
    stack *stack_pass_b = tb->backwardStack;

    // Copy the forward stack into a reversed auxiliary stack
    stack *aux_stack = NULL;
    while (stack_pass_f != NULL) {
        stack *new_stack = (stack*)malloc(sizeof(stack));
        if (!new_stack) {
            return;
        }
        new_stack->info = stack_pass_f->info;
        new_stack->next = aux_stack;
        aux_stack = new_stack;
        stack_pass_f = stack_pass_f->next;
    }

    // Print the forward stack
    stack *print_stack = aux_stack;
    while (print_stack != NULL) {
        fprintf(fout, "%s\n", print_stack->info->url);
        print_stack = print_stack->next;
    }

    fprintf(fout, "%s\n", tb->currentPage->url);

    // Print the backward stack
    while (stack_pass_b != NULL) {
        fprintf(fout, "%s\n", stack_pass_b->info->url);
        stack_pass_b = stack_pass_b->next;
    }

    // Free the auxiliary stack
    stack *tmp_stack = aux_stack;
    while (tmp_stack) {
        stack *tmp = tmp_stack;
        tmp_stack = tmp_stack->next;
        free(tmp);
    }
}

void PRINT(browser *br, FILE *fout) {
    if (!br || !br->list.sentinel || !br->current) {
        return;
    }

    tabWindow *aux = br->list.sentinel->next;

    if (aux->next == br->list.sentinel) {
        fprintf(fout, "%d\n", br->current->id);
        fprintf(fout, "%s\n", br->current->currentPage->description);
    } else {
        fprintf(fout, "%d ", br->current->id);
        // Case: the current tab is the last one
        if (br->current == br->list.sentinel->prev->info) {
            while (aux != br->list.sentinel->prev->prev) {
                fprintf(fout, "%d ", aux->info->id);
                aux = aux->next;
            }
            // Print the last element without a trailing space
            fprintf(fout, "%d", aux->info->id);
        } else {  // Case: the current tab is not the last one
            while (aux->next->info != br->current) {
                aux = aux->next;
            }
            // Print the last element without a trailing space
            tabWindow *ant = aux;
            if (ant == br->list.sentinel) {
                ant = br->list.sentinel->prev;
            }
            aux = aux->next;
            aux = aux->next;
            while (aux != ant) {
                if (aux == br->list.sentinel) {
                    aux = aux->next;
                    continue;
                }
                fprintf(fout, "%d ", aux->info->id);
                aux = aux->next;
            }
            fprintf(fout, "%d", aux->info->id);
        }
        fprintf(fout, "\n");
        fprintf(fout, "%s\n", br->current->currentPage->description);
    }
}

void FreeB(browser *br) {  // Free the browser
    if (!br || !br->list.sentinel)
        return;

    tabWindow *aux = br->list.sentinel->next;
    tabWindow *sentinel = br->list.sentinel;

    while (aux != sentinel) {  // Free every tab
        tabWindow *tab_urm = aux->next;
        tab *tb = aux->info;

        // Free the forward stack
        while (tb->forwardStack) {
            stack *tmp = tb->forwardStack;
            tb->forwardStack = tb->forwardStack->next;
            if (tmp->info) {
                free(tmp->info->description);
                free(tmp->info);
            }
            free(tmp);
        }

        // Free the backward stack
        while (tb->backwardStack) {
            stack *tmp = tb->backwardStack;
            tb->backwardStack = tb->backwardStack->next;
            if (tmp->info) {
                free(tmp->info->description);
                free(tmp->info);
            }
            free(tmp);
        }

        // Free the current page, the tab and its list node
        if (tb->currentPage) {
            free(tb->currentPage->description);
            free(tb->currentPage);
        }

        free(tb);
        free(aux);

        aux = tab_urm;
    }

    free(sentinel);
}
