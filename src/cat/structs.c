//
// Created by Des Caldnd on 11/23/2023.
//
#include "structs.h"
#include <string.h>
#include <stdlib.h>

struct String init_string(const char* src)
{
    int length = strlen(src);
    struct String res;
    res.data = malloc(sizeof(char) * (length + 10));
    if (res.data != NULL)
    {
        res.capacity = length + 10;
        res.size = length;
        strcpy(res.data, src);
    }
    return res;
}

struct String init_string_size(size_t size)
{
    struct String res;
    res.data = malloc(sizeof(char) * size);
    res.size = 0;
    if (res.data != NULL)
    {
        res.capacity = size;
        res.data[0] = '\0';
    }
    else
        res.capacity = 0;
    return res;
}

struct String init_string_from_stream(FILE* stream, int (*is_needed_sym)(int))
{
    struct String res;
    res.data = malloc(sizeof(char) * 10);
    res.size = 0;
    if (res.data != NULL)
    {
        res.capacity = 10;
    } else
    {
        res.capacity = 0;
        return res;
    }
    res.data[0] = '\0';

    char c;

    fscanf(stream, "%c", &c);
    while (!is_needed_sym(c) && c != EOF && c != '\0' && !feof(stream))
        fscanf(stream, "%c", &c);

    while (is_needed_sym(c) && !feof(stream))
    {
        push_string_c(&res, c);
        if (!is_valid_string(&res))
        {
            destroy_string(&res);
            return res;
        }
        fscanf(stream, "%c", &c);
    }

    return res;
}

struct String init_string_from_stream_buf(FILE* stream, char* buf, int (*is_needed_sym)(int), int (*no_skip_sym)(int))
{
    struct String res;
    res.data = malloc(sizeof(char) * 10);
    res.size = 0;
    if (res.data != NULL)
    {
        res.capacity = 10;
    } else
    {
        *buf = 0;
        res.capacity = 0;
        return res;
    }
    res.data[0] = '\0';

    char c;

    fscanf(stream, "%c", &c);
    while (!is_needed_sym(c) && c != EOF && c != '\0' && !no_skip_sym(c) && !feof(stream))
        fscanf(stream, "%c", &c);

    while (is_needed_sym(c) && !feof(stream))
    {
        push_string_c(&res, c);
        if (!is_valid_string(&res))
        {
            *buf = c;
            destroy_string(&res);
            return res;
        }
        fscanf(stream, "%c", &c);
    }

    *buf = c;
    return res;
}

struct String init_string_from_stream_no_skip(FILE* stream, int (*is_needed_sym)(int))
{
    struct String res;
    res.data = malloc(sizeof(char) * 10);
    res.data[0] = '\0';
    res.size = 0;
    if (res.data != NULL)
    {
        res.capacity = 10;
    } else
    {
        res.capacity = 0;
        return res;
    }
    res.data[0] = '\0';

    char c;

    fscanf(stream, "%c", &c);

    while (is_needed_sym(c))
    {
        push_string_c(&res, c);
        if (!is_valid_string(&res))
        {
            destroy_string(&res);
            return res;
        }
        fscanf(stream, "%c", &c);
    }

    return res;
}

void destroy_string(struct String *data)
{
    if (data->data != NULL)
        free(data->data);
    data->data = NULL;
    data->size = 0;
    data->capacity = 0;
}

int is_valid_string(struct String* data)
{
    return data->data != NULL && data->capacity > 0;
}

int is_empty_string(struct String *data)
{
    return data->size <= 0;
}

void realloc_string(struct String* data, int size)
{
    data->data = realloc(data->data, sizeof(char) * size);
    if (is_valid_string(data))
    {
        data->capacity = size;
        if (data->size > data->capacity - 1)
        {
            data->data[data->capacity - 1] = '\0';
            data->size = data->capacity - 1;
        }
    } else
    {
        destroy_string(data);
    }
}

void push_string_c(struct String* data, char c)
{
    if (data->size > data->capacity - 2)
        realloc_string(data, data->capacity * 2);

    if (is_valid_string(data))
    {
        data->data[data->size] = c;
        data->data[data->size + 1] = '\0';
        ++data->size;
    }
}

void cat_string(struct String* dest, const struct String* src)
{
    if (dest->size + src->size > dest->capacity - 1)
    {
        realloc_string(dest, dest->size + src->size + (dest->size < src->size ? dest->size : src->size) + 1);
    }
    if (is_valid_string(dest))
    {
        strcpy(dest->data + dest->size, src->data);
        dest->size += src->size;
    }
}

int cmp_length_string(struct String* lhs, struct String* rhs)
{
    if (lhs->size != rhs->size)
    {
        return lhs->size < rhs->size ? -1 : 1;
    } else
        return strcmp(lhs->data, rhs->data);
}

int cmp_string(struct String* lhs, struct String* rhs)
{
    return strcmp(lhs->data, rhs->data);
}

void cpy_string(struct String* dest, const struct String* src)
{
    if (src->size > dest->capacity - 1)
    {
        realloc_string(dest, src->capacity);
    }
    if (is_valid_string(dest))
    {
        strcpy(dest->data, src->data);
        dest->size = src->size;
    }
}

struct String* cpy_dyn_string(const struct String* src)
{
    struct String* res = malloc(sizeof(struct String));
    if (res != NULL)
    {
        res->size = 0;
        res->capacity = 0;
        res->data = NULL;
        cpy_string(res, src);
    }
    return res;
}