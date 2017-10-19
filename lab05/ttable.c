#include <search.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct tabent {
  char *key;
  long  data;
};

// Tree table routines, based on tsearch et al.
static void *root = NULL;

// comparison used to sort the tree
static int kcomp(const void *a1, const void *a2) {
  struct tabent *t1 = (struct tabent *)a1;
  struct tabent *t2 = (struct tabent *)a2;
  return strcmp(t1->key, t2->key);
}

// initialize the tree
void ttable_init() {
  root = NULL;
}

// insert an entry into the hash table
int ttable_insert(char *key) {
  struct tabent *te = malloc(sizeof(struct tabent));
  te->key = key;
  te->data = 0;
  void *nodep = tsearch((void *)te, &root, kcomp);
  if (!nodep) {
    perror("tsearch");
    return 0;
  }
  struct tabent *realte = *(struct tabent **)nodep;
  if (te == realte) {
    // we need to malloc the string
    te->key = (char *)malloc(strlen(key)+1);
    strcpy(te->key, key);
    te->data = 1;
  } else {
    realte->data++;
    free(te);
  }
  return 1;
}

static void print_tabent(const void *nodep, VISIT value, int level) {
  struct tabent *te = *(struct tabent **)nodep;
  if (value == preorder || value == leaf) {
    printf("%s %ld\n", te->key, te->data);
  }
  (void)level;
}

// print out all the hash-table entries, in the order given by the items array
void ttable_list() {
  twalk(root, print_tabent);
}

int ttable_destroy() {
  while (root) {
    struct tabent *te = *(struct tabent **)root;
    tdelete((void *)te, &root, kcomp);
    free(te->key);
    free(te);
  }
  return 0;
}
