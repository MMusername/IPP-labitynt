#ifndef PATH_FINDING_H
#define PATH_FINDING_H

struct Labyrintht {
    unsigned char *lab;
    size_t labyrinth_size;
};
typedef struct Labyrintht Labyrinth;

extern void find_path();

#endif
