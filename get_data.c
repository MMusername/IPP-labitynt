/*
 *  GET DATA MODULE
 */

#include <stdio.h>
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <stddef.h>

#include "get_data.h"

#define MOD 4294967296

/*
 *  true for all white signs, without '\n'
 */
static bool is_space(char c) {
    return (isspace(c) && c != '\n');
}

/*
 *  gets next not-white sign
 */
static char get_char() {
    char c = getchar();
    while (is_space(c)) {
        c = getchar();
    }
    return c;
}

/*
 *  true end of the line
 */
static bool is_end(char c) {
    return c == '\n' || c == EOF;
}


 /*
 *  gets single size_t number
 */
static size_t get_number(char *last_char, bool *end, bool *is_error) {
    char c = (*last_char);
    size_t number = 0;
    (*is_error) = false;

    while (isdigit(c)) {
        if (SIZE_MAX / 10 < number) {
            (*is_error) = true;
        }
        number *= 10;
        if (SIZE_MAX - (c - '0') < number) {
            (*is_error) = true;
        } 
        number += c - '0';
        c = getchar();
    }

    if (is_space(c)) {
        c = get_char();
    }
    if (is_end(c)) {
        (*end) = true;
    }
    (*last_char) = c;

    return number; 
}

/*
 *  returns bigger array_size
 */
static size_t new_size(unsigned long long size) {
    if (SIZE_MAX / 2 + 1 <= size)    return SIZE_MAX;
    else    return size * 2 + 1;
}

/*
 *  gets first line
 */
static void get_first_line(size_t **a, size_t *an,
        size_t *dimensions_product, short *error) {
    size_t *array = NULL;
    bool is_error;
    size_t size = 0;
    bool end = false;
    size_t i = 0;
    char last_char = get_char();

    while (!end && (*error) == -1) {
        size_t x = get_number(&last_char, &end, &is_error);
        if (x == 0 || (*dimensions_product) >= SIZE_MAX / x || is_error) {
            (*error) = 1;
        }
        (*dimensions_product) *= x;
        if (i == size) {
            size = new_size(size);
            array = realloc(array, (size_t) size * sizeof(size_t));
            if (array == NULL) {
                fprintf(stderr, "ERROR 0\n");
                exit(1);
            }
        }
        array[i] = x;
        i++;
    }

    (*a) = realloc(array, (size_t) i * sizeof(size_t));
    if (*a == NULL) {
        fprintf(stderr, "ERROR 0\n");
        exit(1);
    }
    (*an) = i;
}

/*
 *  gets the line of a certain size (for 2nd and 3rd data line)
 */
static void get_line(size_t **a, size_t n, short *error, int line) {
    bool is_error = false;
    (*a) = malloc((size_t) n * sizeof(size_t));
    if (*a == NULL) {
        fprintf(stderr, "ERROR 0\n");
        exit(1);
    }
    bool end = false;
    char last_char = get_char();

    for (size_t i = 0; i < n; i++) {
        size_t temp = get_number(&last_char, &end, &is_error);
        if ((end == true && i != n - 1) || temp == 0 || is_error == true) {
            (*error) = line;
            break;
        }
        else {
            (*a)[i] = temp;
        }
    }

    if (!is_end(last_char)) {
        (*error) = line;
    }
}

/*
 *  changes signs 0 - F (hex)
 *  to number 0 - 15 (dec)
 */
static int hex_char_to_dec_int(char c) {
    if (c >= 'A' && c <= 'F')    return c - 'A' + 10;
    else if (c >= 'a' && c <= 'f')   return c - 'a' + 10;
    else    return c - '0';
}
/*
 *  char to dec
 */
static uint32_t dec_char_to_dec_uint32(char c) {
    return c - '0';
}

/*
 *  mirrors bits in a single char
 */
static char mirror_bits(char c) {
    c = (c & 0xF0) >> 4 | (c & 0x0F) << 4;
    c = (c & 0xCC) >> 2 | (c & 0x33) << 2;
    c = (c & 0xAA) >> 1 | (c & 0x55) << 1;
    return c;
}

/*
 *  omits leading zeros
 */
static char no_zero() {
    char c = getchar();
    while (c == '0') {
        c = getchar();
    }
    return c;
}
/*
 *  mirror bits in array
 */
static void mirror_labyrinth(unsigned char **labyrinth, 
        size_t labyrinth_size, size_t dimensions_product) {
    for (size_t i = 0; i < labyrinth_size / 2; i++) {
        unsigned char temp = (*labyrinth)[i];
        (*labyrinth)[i] = (*labyrinth)[labyrinth_size - 1 - i];
        (*labyrinth)[labyrinth_size - 1 - i] = temp;
    }

    for (size_t i = 0; i < labyrinth_size; i++) {
        (*labyrinth)[i] = mirror_bits((*labyrinth)[i]);
    }

    // shifts the array when necessary
    if (dimensions_product % 8 != 0) {
        for (size_t i = 0; i < labyrinth_size * 8 - 4; i++) {
            if ((*labyrinth)[(i + 4) / 8] & (1 << ((i + 4) % 8))) {
                (*labyrinth)[i / 8] |= (1 << (i % 8));
            }
            else {
                (*labyrinth)[i / 8] &= (1 << 8) - (1 << (i % 8)) - 1;
            }
        }
    }
}

/*
 *  (hex) reads 4th line and saves in as bits in unsigned char array
 */
static void hex_get_labyrinth(unsigned char **labyrinth,
        size_t dimensions_product, short *error) {
    size_t labyrinth_size;
    labyrinth_size = dimensions_product / 8 + (dimensions_product % 8 != 0);
    (*labyrinth) = calloc(labyrinth_size, sizeof(char));
    if ((*labyrinth) == NULL) {
        fprintf(stderr, "ERROR 0\n");
        exit(1);
    }
    char c = getchar();
    if (!isxdigit(c)) {
        (*error) = 4;
    } else if (c == '0') {
        c = no_zero();
        if (!isdigit(c) && !is_end(c)) {
            if (!is_space(c)) {
                (*error) = 4;
            }
            c = get_char();
            if (!is_end(c)) {
                (*error) = 4;
            }
        }
    }

    size_t index = 0;
    while (isxdigit(c) && index < labyrinth_size * 8) {
        int x = hex_char_to_dec_int(c);
        for (int i = 3; i >= 0; i--) {
            if (x % 2 == 1) {
                (*labyrinth)[(index + i) / 8] |= 1 << (index + i) % 8;
            }
            x /= 2;
        }
        index += 4;
        c = getchar();
        if (is_space(c)) {
            c = get_char();
            if (!is_end(c))   (*error) = 4;
        } 
    }

    if (!is_space(c) && !is_end(c)) {
        (*error) = 4;
    }
    else if (c == '\n') {
        c = getchar();
        if (c != EOF) {
            (*error) = 5;
        }
    }

    mirror_labyrinth(labyrinth, labyrinth_size, dimensions_product);
}

/*
 *  reads uint32 number
 */
static uint32_t get_uint32_number(char *last_char, bool *is_error) {
    uint32_t number = 0;
    char c = get_char();
    if (!isdigit(c)) {
        (*is_error) = true;
    }

    while (isdigit(c)) {
        uint32_t x = dec_char_to_dec_uint32(c);
        if (number > UINT32_MAX / 10) {
            (*is_error) = true;
        }
        number *= 10;
        if (number > UINT32_MAX - x) {
            (*is_error) = true;
        }
        number += x;
        c = getchar();
    }

    (*last_char) = c;
    return number;
}

/*
 *  (R) gets numbers from 4th line
 */
static void r_get_numbers(uint32_t *a, uint32_t *b, uint32_t *m,
        uint32_t *r, uint32_t *s_0, short *error) {

    bool is_error = false;
    char last_char;
    (*a) = get_uint32_number(&last_char, &is_error);
    if (is_error == false || is_end(last_char)) {
        (*b) = get_uint32_number(&last_char, &is_error);
    }
    if (is_error == false || is_end(last_char)) {
        (*m) = get_uint32_number(&last_char, &is_error);
        if ((*m) == 0) {
            (*error) = 4;
        }
    }
    if (is_error == false || is_end(last_char)) {
        (*r) = get_uint32_number(&last_char, &is_error);
    }
    if (is_end(last_char)) {
        (*error) = 4;
    }
    if (is_error == false || is_end(last_char)) {
        (*s_0) = get_uint32_number(&last_char, &is_error);
    }

    if (is_error == true) {
        (*error) = 4;
    }
    else if (last_char == '\n') {
        last_char = getchar();
        if (last_char != EOF) {
            (*error) = 5;
        }
    }
    else if (last_char != EOF) {
        last_char = get_char();
        if (last_char == '\n') {
            last_char = getchar();
            if (last_char != EOF) {
                (*error) = 5;
            }
        }
        else if (last_char != EOF) {
            (*error) = 4;
        }
    }
}

/*
 *  calulates w_0, ..., w_r and saves into the array
 */
static void calculate_w_series(uint32_t a, uint32_t b, uint32_t m, uint32_t r,
        uint32_t s, size_t dimensions_product, uint32_t **series_array) {
    uint32_t uint_dimensions_product = (uint32_t) dimensions_product;
    *series_array = malloc((size_t) r * sizeof(uint32_t));
    if ((*series_array) == NULL) {
        fprintf(stderr, "ERROR 0\n");
        exit(1);
    }
    (*series_array)[0] = ((a % m) * (s % m) + (b % m)) % m;

    for (uint32_t i = 1; i < r; i++) {
        (*series_array)[i] = ((*series_array)[i - 1] * (a % m) + (b % m)) % m;
        (*series_array)[i - 1] %= dimensions_product;
    }    
    (*series_array)[r - 1] %= uint_dimensions_product;
}

/*
 *  (R) gets 4th line and saves as bits in unsigned char array
 */
static void r_get_labyrinth(unsigned char **labyrinth,
        size_t dimensions_product, short *error) {
    uint32_t a, b, m, r, s_0;
    r_get_numbers(&a, &b, &m, &r, &s_0, error);
    if ((*error) == -1) {
        uint32_t *series_array;
        calculate_w_series(a, b, m, r, s_0, dimensions_product, &series_array);

        size_t labyrinth_size;
        labyrinth_size = dimensions_product / 8;
        labyrinth_size += (dimensions_product % 8 != 0);
        (*labyrinth) = calloc(labyrinth_size, sizeof(char));
        if ((*labyrinth) == NULL) {
            fprintf(stderr, "ERROR 0\n");
            exit(1);
        }

        for (uint32_t i = 0; i < r; i++) {
            size_t bit_number = (size_t) series_array[i];
            while (bit_number < dimensions_product) {
                (*labyrinth)[bit_number / 8] |= 1 << bit_number % 8;
                bit_number += MOD;
            }
        }

        free(series_array);
    }
}

/*
 *  Gets data
 */
void get_data(size_t **a, size_t **b, size_t **c, unsigned char **d,
        size_t *n, size_t *dimensions_product, short *error) {
    *dimensions_product = 1;
    get_first_line(a, n, dimensions_product, error);    
    size_t constant_size = (*n);
    if ((*error) == -1) {
        get_line(b, constant_size, error, 2);
        if ((*error) == -1) {
            for (size_t i = 0; i < constant_size; i++) {
                if ((*a)[i] < (*b)[i]) {
                    (*error) = 2;
                }
            }
        }
    }
    if ((*error) == -1) {
        get_line(c, constant_size, error, 3);
        if ((*error) == -1) {
            for (size_t i = 0; i < constant_size; i++) {
                if ((*a)[i] < (*c)[i]) {
                    (*error) = 3;
                }
            }
        }
    }
    if ((*error) == -1) {
        char temp = get_char();
        if (temp == '0') {
            temp = getchar();
            if (temp != 'x') {
                (*error) = 4;
            }
            else {
                hex_get_labyrinth(d, *dimensions_product, error);
            }
        }
        else if (temp == 'R') {
            r_get_labyrinth(d, *dimensions_product, error);
        }
        else {
            (*error) = 4;
        }
    }
}

