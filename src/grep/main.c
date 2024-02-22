//
// Created by Des Caldnd on 2/21/2024.
//
#include "structs.h"
#include <stdio.h>
#include <string.h>
#include <regex.h>

#define FLAG_ERROR 3
#define FLAG_OK 1
#define NOT_FLAG 0
#define SOURCE_FLAG -1
#define FILE_SRC_FLAG -2

typedef unsigned char uc;
VECTOR(str, struct String)

enum pattern_source
{ STANDART, FLAGS };

struct flag_state
{
    enum pattern_source source;
    uc state;
};

int get_flag_type(char* str, struct flag_state* state);
int process_by_symbol(char sym, struct flag_state* state);

struct vector_str from_one_pattern(const char* pattern);
struct vector_str from_flag(char** args, size_t count);
int from_file(const char* filepath, struct vector_str* res);
int from_pattern(char* str, struct vector_str* res);
struct String concat_vec(struct vector_str* vec);

int is_i(struct flag_state state);
int is_v(struct flag_state state);
int is_c(struct flag_state state);
int is_l(struct flag_state state);
int is_n(struct flag_state state);
int is_h(struct flag_state state);
int is_s(struct flag_state state);
int is_o(struct flag_state state);

void destroy_str_vec(struct String* str, void* n);

void work(FILE* in, struct flag_state state, regex_t reg);

void work_standart(FILE* in, struct flag_state state, regex_t reg);
void work_count(FILE* in, struct flag_state state, regex_t reg);
void work_file(FILE* in, struct flag_state state, regex_t reg);
void work_o(FILE* in, struct flag_state state, regex_t reg);

int main(int argc, char* argv[])
{
    struct flag_state state  = {STANDART, 0};
    size_t pattern_count = 0;

    int i = 1;

    for (; i < argc; ++i)
    {
        int res = get_flag_type(argv[i], &state);
        if (res == SOURCE_FLAG || res == FILE_SRC_FLAG)
        {
            ++pattern_count;
            ++i;
        } else if (res == NOT_FLAG)
        {
            break;
        } else if (res == FLAG_ERROR)
        {
            printf("Error in flags\n");
            return 0;
        }
    }

    struct vector_str regs;

    switch (state.source)
    {
        case STANDART:
            regs = from_one_pattern(argv[i]);
            ++i;
            break;
        case FLAGS:
            regs = from_flag(argv, pattern_count);
            break;
    }

    if (!is_valid_vector_str(&regs))
    {
        printf("Bad alloc\n");
        return 0;
    }

    struct String str = concat_vec(&regs);

    for_all_vector_str(&regs, destroy_str_vec, NULL);
    destroy_vector_str(&regs);

    if (!is_valid_string(&str))
    {
        printf("Bad alloc\n");
        return 0;
    }

    regex_t regex;

    int value = regcomp(&regex, str.data, (is_i(state) ? REG_ICASE : 0) | REG_EXTENDED);

    destroy_string(&str);

    if (value != 0)
    {
        printf("Error while compiling regex\n");
        return 0;
    }

    for (; i < argc; ++i)
    {
        FILE* in = fopen(argv[i], "r");
        if (in == NULL)
        {
            if (!is_s(state))
                printf("File %s could not be opened\n", argv[i]);
            continue;
        }

        work(in, state, regex);

        fclose(in);
    }

    regfree(&regex);

    return 0;
}

int get_flag_type(char* str, struct flag_state* state)
{
    int len = strnlen(str, 4);
    if (len > 3 || len < 2 || str[0] != '-')
        return NOT_FLAG;

    if (len == 2)
    {
        return process_by_symbol(str[1], state);
    } else
    {
        if (str[1] == str[2])
            return FLAG_ERROR;
        int v1 = process_by_symbol(str[1], state);
        int v2 = process_by_symbol(str[2], state);


        if (v1 == NOT_FLAG || v2 == NOT_FLAG)
            return NOT_FLAG;
        if ((v1 == SOURCE_FLAG && v2 == FILE_SRC_FLAG) || (v1 == FILE_SRC_FLAG && v2 == SOURCE_FLAG))
            return FLAG_ERROR;
        if (v1 == SOURCE_FLAG || v2 == SOURCE_FLAG)
            return SOURCE_FLAG;
        if (v1 == FILE_SRC_FLAG || v2 == FILE_SRC_FLAG)
            return FILE_SRC_FLAG;
        return FLAG_OK;
    }
}

int process_by_symbol(char sym, struct flag_state* state)
{
    int res = FLAG_OK;
    switch (sym)
    {
        case 'e':
            state->source = FLAGS;
            res = SOURCE_FLAG;
            break;
        case 'i':
            state->state |= 1u;
            break;
        case 'v':
            state->state |= 1u << 1;
            break;
        case 'c':
            state->state |= 1u << 2;
            break;
        case 'l':
            state->state |= 1u << 3;
            break;
        case 'n':
            state->state |= 1u << 4;
            break;
        case 'h':
            state->state |= 1u << 5;
            break;
        case 's':
            state->state |= 1u << 6;
            break;
        case 'f':
            state->source = FLAGS;
            res = FILE_SRC_FLAG;
            break;
        case 'o':
            state->state |= 1u << 7;
            break;
        default:
            res = NOT_FLAG;
            break;
    }
    return res;
}

struct vector_str from_one_pattern(const char* pattern)
{
    struct vector_str res = init_vector_str(1);
    if (!is_valid_vector_str(&res))
    {
        return res;
    }

    struct String str = init_string(pattern);

    if (!is_valid_string(&str))
    {
        destroy_vector_str(&res);
        return res;
    }

    push_vector_str(&res, str, destroy_str_vec);
    return res;
}

int always_needed(int c)
{
    return c && 1;
}

int from_file(const char* filepath, struct vector_str* res)
{
    FILE* in = fopen(filepath, "r");

    if (in == NULL)
    {
        printf("Cannot be opened source file: %s\n", filepath);
        return 0;
    }

    struct String str = init_string_from_stream(in, always_needed);

    fclose(in);

    if (!is_valid_string(&str))
    {
        return 0;
    }

    push_vector_str(res, str, destroy_str_vec);
    return 1;
}

int from_pattern(char* str, struct vector_str* res)
{
    struct String string = init_string(str);

    if (!is_valid_string(&string))
    {
        return 0;
    }

    push_vector_str(res, string, destroy_str_vec);
    return 1;
}

struct vector_str from_flag(char** args, size_t count)
{
    struct vector_str res = init_vector_str(count);

    if (!is_valid_vector_str(&res))
    {
        return res;
    }

    size_t counter = 1, i = 1;
    struct flag_state st;

    while (counter < count)
    {
        int val = get_flag_type(args[i], &st);
        if (val == SOURCE_FLAG || val == FILE_SRC_FLAG)
        {
            ++i;
            ++counter;
            int value;
            if (val == SOURCE_FLAG)
                value = from_pattern(args[i], &res);
            else
                value = from_file(args[i], &res);

            if (value != 0)
            {
                for_all_vector_str(&res, destroy_str_vec, 0);
                destroy_vector_str(&res);
                return res;
            }
        }
        ++i;
    }

    return res;
}

struct String concat_vec(struct vector_str* vec)
{
    size_t size = 0;

    for (size_t i = 0; i < vec->size; ++i)
    {
        size += vec->data[i].size;
    }

    size += vec->size * 3 + 4;

    struct String res = init_string_size(size);

    if (!is_valid_string(&res))
        return res;

    push_string_c(&res, '(');
    cat_string(&res, vec->data);
    push_string_c(&res, ')');

    for (size_t i = 0; i < vec->size; ++i)
    {
        push_string_c(&res, '(');
        cat_string(&res, vec->data + i);
        push_string_c(&res, ')');
    }

    return res;
}

void destroy_str_vec(struct String* str, void* n)
{
    n = 0;
    destroy_string(str + (size_t) n);
}

void work(FILE* in, struct flag_state state, regex_t reg)
{
    if (is_l(state))
        work_file(in, state, reg);
    else if (is_c(state))
        work_count(in, state, reg);
    else if (is_o(state))
        work_o(in, state, reg);
    else
        work_standart(in, state, reg);
}

void work_standart(FILE* in, struct flag_state state, regex_t reg)
{
    
}

void work_count(FILE* in, struct flag_state state, regex_t reg)
{
    
}

void work_file(FILE* in, struct flag_state state, regex_t reg)
{
    
}

void work_o(FILE* in, struct flag_state state, regex_t reg)
{
    
}

int is_i(struct flag_state state)
{
    return state.state & (1u);
}

int is_v(struct flag_state state)
{
    return state.state & (1u << 1);
}

int is_c(struct flag_state state)
{
    return state.state & (1u << 2);
}

int is_l(struct flag_state state)
{
    return state.state & (1u << 3);
}

int is_n(struct flag_state state)
{
    return state.state & (1u << 4);
}

int is_h(struct flag_state state)
{
    return state.state & (1u << 5);
}

int is_s(struct flag_state state)
{
    return state.state & (1u << 6);
}

int is_o(struct flag_state state)
{
    return state.state & (1u << 7);
}