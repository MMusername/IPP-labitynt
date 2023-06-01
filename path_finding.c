#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "path_finding.h"
#include "get_data.h"
#include "cube_struct.h"
#include "queue.h"

/*
 *  creates cube containing prfix product of dimensons,
 *  form 1 to n_1 * n_2 * ... * n_k-1
 */
static Cube create_prefix_product(Cube max_cube, size_t dimensions_size) {
    Cube prefix_product_cube;
    prefix_product_cube.pos = malloc((size_t) dimensions_size * sizeof(size_t));
    prefix_product_cube.pos[0] = 1;

    for (size_t i = 1; i < dimensions_size; i++) {
        size_t x = prefix_product_cube.pos[i - 1] * max_cube.pos[i - 1];
        prefix_product_cube.pos[i] = x;
    }

    return prefix_product_cube;
}

/*
 *  calculates labirynth index for given cube
 */
static size_t labyrinth_index(Cube cube, Cube prefix_product_cube,
        size_t dimensions_size) {
    size_t labyrinth_pos = 0;
    for (size_t i = 0; i < dimensions_size; i++) {
        labyrinth_pos += (cube.pos[i] - 1) * prefix_product_cube.pos[i];
    }
    return labyrinth_pos;
}

/*
 *  checks if cube is free
 */
static bool is_cube_available(Labyrinth labyrinth, size_t labyrinth_pos) {
    return (labyrinth.lab[labyrinth_pos / 8] & 1 << labyrinth_pos % 8) == 0;
}

/*
 *  mark a cube as visited
 */
static void mark_as_unavailable(Labyrinth *labyrinth, size_t labyrinth_pos) {
    (*labyrinth).lab[labyrinth_pos / 8] |= 1 << labyrinth_pos % 8;
}

/*
 *  calulates the cube coordinates from the labirynth index
 */
static Cube cube_from_index(Cube max_cube, size_t dimensions_size, size_t index) {
    size_t *cube_array = malloc((size_t) dimensions_size * sizeof(size_t));

    for (size_t i = 0; i < dimensions_size - 1; i++) {
        size_t x = index % max_cube.pos[i];
        cube_array[i] = x + 1;
        index -= x;
        index /= max_cube.pos[i];
    }

    cube_array[dimensions_size - 1] = index + 1;
    Cube cube;
    cube.pos = cube_array;
    return cube;
}

/*
 *  BFS algorithm
 *  searching for the shortest path in the labirynth
 */
static size_t bfs(Labyrinth labyrinth, size_t start_index, size_t finish_index,
        Cube max_cube, Cube prefix_product_cube,
        size_t dimensions_size, bool *is_no_way) {
    (*is_no_way) = true;
    int queue_number = 0;
    Queue **q = (Queue **)malloc((size_t) 2 * sizeof(Queue));
    q[0] = q_init();
    q[1] = q_init();
    push(q[0], start_index);
    mark_as_unavailable(&labyrinth, start_index);
    size_t result = 0;

    bool is_bfs_finished = false;
    while ((!is_empty(*q[0]) || !is_empty(*q[1])) && !is_bfs_finished) {
        while (!is_empty(*q[queue_number]) && !is_bfs_finished) {
            size_t current_cube_index = pop(q[queue_number]);
            if (current_cube_index == finish_index) {
                is_bfs_finished = true;
                (*is_no_way) = false;
            }
            Cube current_cube = cube_from_index(max_cube, dimensions_size, current_cube_index);
            for (size_t i = 0; i < dimensions_size; i++) {
                if (current_cube.pos[i] > 1) {
                    current_cube.pos[i]--;
                    size_t cube_index = labyrinth_index(current_cube, 
                            prefix_product_cube, dimensions_size);
                    if (is_cube_available(labyrinth, cube_index)) {
                        push(q[1 - queue_number], cube_index);
                        mark_as_unavailable(&labyrinth, cube_index);
                    }
                    current_cube.pos[i]++;
                }
                if (current_cube.pos[i] < max_cube.pos[i]) {
                    current_cube.pos[i]++;
                    size_t cube_index = labyrinth_index(current_cube,
                            prefix_product_cube, dimensions_size);
                    if (is_cube_available(labyrinth, cube_index)) {
                        push(q[1 - queue_number], cube_index);
                        mark_as_unavailable(&labyrinth, cube_index);
                    }
                    current_cube.pos[i]--;
                }
            }
            free(current_cube.pos);
        }
        result++;
        queue_number = 1 - queue_number;
    }
    while (!is_empty(*q[0])) {
        pop(q[0]);
    }
    while (!is_empty(*q[1])) {
        pop(q[1]);
    }
    free(q[0]);
    free(q[1]);
    free(q);
    return result - 1;
}

/*
 *  free the allocated data, when an error occurred
 */
static void error_action(Cube max_cube, Cube start, Cube finish,
        Labyrinth labyrinth, short error) {
    fprintf(stderr, "ERROR %d\n", error);
    free(max_cube.pos);
    free(start.pos);
    free(finish.pos);
    free(labyrinth.lab);
    exit(1);
}

/*
 *  gets the data and finds the shortest path in the labirynth
 */
void find_path() {
    Labyrinth labyrinth;
    labyrinth.lab = NULL;
    Cube max_cube;
    max_cube.pos = NULL;
    Cube start;
    start.pos = NULL;
    Cube finish;
    finish.pos = NULL;
    size_t dimensions_size = 0;
    size_t dimensions_product;
    short error = -1;

    get_data(&max_cube.pos, &start.pos, &finish.pos, &labyrinth.lab,
            &dimensions_size, &dimensions_product, &error);
    if (error != -1) {
        error_action(max_cube, start, finish, labyrinth, error);
    }
    labyrinth.labyrinth_size = dimensions_product;
    bool is_no_way;
    Cube prefix_product_cube;
    prefix_product_cube = create_prefix_product(max_cube, dimensions_size);
    size_t start_index;
    start_index = labyrinth_index(start, prefix_product_cube, dimensions_size);

    if (!is_cube_available(labyrinth, start_index)) {
        error = 2;
    }
    size_t finish_index;
    finish_index = labyrinth_index(finish, prefix_product_cube, dimensions_size);
    if (error == -1 && !is_cube_available(labyrinth, finish_index)) {
        error = 3;
    }
    if (error != -1) {
        free(prefix_product_cube.pos);
        error_action(max_cube, start, finish, labyrinth, error);
    }

    size_t result = bfs(labyrinth, start_index, finish_index, max_cube,
            prefix_product_cube, dimensions_size, &is_no_way);
    
    if (is_no_way) {
        printf("NO WAY!");
    } else {
        printf("%lu\n", result);
    }

    free(prefix_product_cube.pos);
    free(max_cube.pos);
    free(start.pos);
    free(finish.pos);
    free(labyrinth.lab);
}

