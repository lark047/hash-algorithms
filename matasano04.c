#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <inttypes.h>

#include "matasano.h"
#include "matasano-common.h"

/* string length (60) + newline + nul byte */
#define BUFSIZE (60 + 1 + 1)

struct node
{
    struct node *next;
    struct result *result;
};

static void add_node(struct node **, struct node *);
// static void print_list(struct node *);
static void delete_list(struct node **);

struct result *DecodeXORFromFile(FILE *fp)
{
    char line[BUFSIZE], *p; /* TODO remove hard-coded size */
    uint8_t * const hex = malloc(30 * sizeof *hex); /* TODO ^ and check */
    struct result *result;

    struct node *head = NULL, *node;

    while (fgets(line, BUFSIZE, fp) != NULL)
    {
        if ((p = strchr(line, '\n')) != NULL)
        {
            *p = 0;
        }

        if ((p = strchr(line, '\r')) != NULL)
        {
            *p = 0;
        }

        print_d("read \"%s\"\n", line);

        StringToHex(line, hex);
        // PrintHex(hex, 30);

        result = DecodeXOR(hex, 30);

        print_d("decoded with '%c' (0x%02x): %s\n", (isprint(result->key) ? result->key : ' '), result->key, result->text);

        node = malloc(sizeof *node); /* TODO check */
        node->next = NULL;
        node->result = result;

        add_node(&head, node);
    }

    result = head->result;

    // print_list(head);
    delete_list(&head->next);
    free(head);
    free(hex);

    return result;
}

static void add_node(struct node **head_ref, struct node *node)
{
#ifdef DEBUG
    uint8_t index = 0;
#endif

    if ((*head_ref) == NULL)
    {
        print_d("%s\n", "adding node at head (previously empty list)");
        *head_ref = node;
    }
    else
    {
        struct node *current = *head_ref, *prev = NULL;

        while (current && current->result->score > node->result->score)
        {
#ifdef DEBUG
            ++index;
#endif

            prev = current;
            current = current->next;
        }

        print_d("adding node at position %" PRIu8 "\n", index);

        if (prev == NULL)
        {
            node->next = *head_ref;
            *head_ref = node;
        }
        else
        {
            prev->next = node;
        }

        node->next = current;
    }
}

#if 0
static void print_list(struct node *head)
{
    struct node *current = head;
    struct result *result;

    while (current)
    {
        result = current->result;
        printf("Node with key '%c' and score %.5f produced \"%s\"\n", (isprint(result->key) ? result->key : ' '), result->score, result->text);
        current = current->next;
    }
}
#endif

static void delete_list(struct node **head)
{
    struct node *current = *head;

    while (current)
    {
        struct node *next = current->next;

        free(current->result->hex);
        free(current->result->text);
        free(current->result);
        free(current);

        current = next;
    }
}
