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

typedef unsigned char uc;
VECTOR(reg, regex_t)

enum pattern_source
{ STANDART, FLAGS, FILE_ };

struct flag_state
{
    enum pattern_source source;
    uc state;
};

int get_flag_type(char* str, struct flag_state* state);
int process_by_symbol(char sym, struct flag_state* state);

struct vector_reg from_one_pattern(const char* pattern);
struct vector_reg from_file(const char* filepath);
struct vector_reg from_flag_pattern(char** args, size_t count);

void destroy_regex(regex_t* regex, void*);

void work(FILE* in, struct flag_state state, struct vector_reg regs);

int main(int argc, char* argv[])
{
    struct flag_state state  = {STANDART, 0};
    size_t pattern_count = 0;
    char* filepath = NULL;

    int i = 1;

    for (; i < argc; ++i)
    {
        int res = get_flag_type(argv[i], &state);
        if (res == SOURCE_FLAG)
        {
            ++pattern_count;
            ++i;
            filepath = argv[i];
        } else if (res == NOT_FLAG)
        {
            break;
        } else if (res == FLAG_ERROR)
        {
            printf("Error in flags\n");
            return 0;
        }
    }

    struct vector_reg regs;

    switch (state.source)
    {
        case STANDART:
            regs = from_one_pattern(argv[i]);
            ++i;
            break;
        case FLAGS:
            regs = from_flag_pattern(argv, pattern_count);
            break;
        case FILE_:
            regs = from_file(filepath);
            break;
    }

    if (!is_valid_vector_reg(&regs))
    {
        printf("Bad alloc\n");
        return 0;
    }

    for (; i < argc; ++i)
    {
        FILE* in = fopen(argv[i], "r");
        if (in == NULL)
        {
            printf("File %s could not be opened\n", argv[i]);
            continue;
        }

        work(in, state, regs);

        fclose(in);
    }

    for_all_vector_reg(&regs, destroy_regex, NULL);
    destroy_vector_reg(&regs);

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

        if (v1 == FLAG_ERROR || v2 == FLAG_ERROR)
            return FLAG_ERROR;
        if (v1 == SOURCE_FLAG || v2 == SOURCE_FLAG)
            return SOURCE_FLAG;
        return FLAG_OK;
    }
}

int process_by_symbol(char sym, struct flag_state* state)
{

}

struct vector_reg from_one_pattern(const char* pattern)
{
    struct vector_reg res = init_vector_reg(1);
    if (!is_valid_vector_reg(&res))
    {
        return res;
    }

    regex_t regex;
    int value = regcomp(&regex, pattern, 0);

    if (value != 0)
    {
        destroy_vector_reg(&res);
        return res;
    }

    push_vector_reg(&res, regex, destroy_regex);
    return res;
}

int always_needed(int c)
{
    return c && 1;
}

struct vector_reg from_file(const char* filepath)
{
    struct vector_reg res = {NULL, 0, 0};
    FILE* in = fopen(filepath, "r");

    if (in == NULL)
    {
        printf("Cannot be opened source file: %s\n", filepath);
        return res;
    }

    struct String str = init_string_from_stream(in, always_needed);

    fclose(in);

    if (!is_valid_string(&str))
    {
        return res;
    }

    regex_t regex;

    int value = regcomp(&regex, str.data, 0);

    destroy_string(&str);

    if (value != 0)
    {
        return res;
    }

    res = init_vector_reg(1);

    if (!is_valid_vector_reg(&res))
    {
        regfree(&regex);
        return res;
    }

    push_vector_reg(&res, regex, destroy_regex);
    return res;
}

struct vector_reg from_flag_pattern(char** args, size_t count)
{
    struct vector_reg res = init_vector_reg(count);

    if (!is_valid_vector_reg(&res))
    {
        return res;
    }

    size_t counter = 1, i = 1;
    struct flag_state st;

    while (counter < count)
    {
        if (get_flag_type(args[i], &st) == SOURCE_FLAG)
        {
            ++i;
            ++counter;
            regex_t regex;
            int value = regcomp(&regex, args[i], 0);

            if (value != 0)
            {
                for_all_vector_reg(&res, destroy_regex, 0);
                destroy_vector_reg(&res);
                return res;
            }

            push_vector_reg(&res, regex, destroy_regex);
        }
        ++i;
    }

    return res;
}

void destroy_regex(regex_t* regex, void* n)
{
    n = 0;
    regfree(regex + (size_t) n);
}

void work(FILE* in, struct flag_state state, const struct vector_reg regs)
{

}