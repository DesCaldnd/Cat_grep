//
// Created by Des Caldnd on 2/21/2024.
//
#include "structs.h"
#include <stdio.h>
#include <regex.h>

typedef unsigned char uc;

enum pattern_source
{ STANDART, FLAGS, FILE_ };

struct flag_state
{
    enum pattern_source source;
    uc state;
};

int get_flag_type(char* str, struct flag_state* state);
int process_by_symbol(char* str, struct flag_state* state);

void work(FILE* in, struct flag_state state);

int main(int argc, char* argv[])
{
    struct flag_state state  = {STANDART, 0};
    size_t pattern_count = 0;

    int i = 1;

    for (; i < argc; ++i)
    {
        int res = get_flag_type(argv[i], &state);
        if (res == -1)
        {
            ++pattern_count;
            ++i;
        } else if (res == 0)
        {
            break;
        }
    }

    return 0;
}
