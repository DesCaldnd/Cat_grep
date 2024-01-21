#include <stdio.h>
#include "structs.h"

enum flag_type
{ B_F, E_F, N_F, S_F, T_F, UNDEF };

enum flag_type get_flag_type(char* str);
void work(FILE* in, enum flag_type type);

void b_work(FILE* in);
void e_work(FILE* in);
void n_work(FILE* in);
void s_work(FILE* in);
void t_work(FILE* in);

int not_endl(int c);

int main (int argc, char* argv[])
{
    if (argc != 3)
    {
        printf("n/a");
        return 0;
    }

    enum flag_type type = get_flag_type(argv[1]);

    if (type == UNDEF)
    {
        printf("n/a");
        return 0;
    }

    FILE* in = fopen(argv[2], "r");

    if (in == NULL)
    {
        printf("n/a");
        return 0;
    }

    work(in, type);

    fclose(in);
    return 0;
}

enum flag_type get_flag_type(char* str)
{
    if (str[0] != '-' || str[2] != '\0')
        return UNDEF;

    if (str[1] == 'b')
        return B_F;
    else if (str[1] == 'e')
        return E_F;
    else if (str[1] == 'n')
        return N_F;
    else if (str[1] == 's')
        return S_F;
    else if (str[1] == 't')
        return T_F;
    else
        return UNDEF;
}

void work(FILE* in, enum flag_type type)
{
    switch (type)
    {
        case B_F:
            b_work(in);
            break;
        case E_F:
            e_work(in);
            break;
        case N_F:
            n_work(in);
            break;
        case S_F:
            s_work(in);
            break;
        default:
            t_work(in);
            break;
    }
}

void b_work(FILE* in)
{
    struct String str;
    size_t size = 1;

    while (!feof(in))
    {
        str = init_string_from_stream(in, not_endl);

        if (!is_valid_string(&str))
            return;

        if (str.size > 0)
        {
            printf("%5zu %s\n", size, str.data);
            ++size;
        } else
        {
            printf("\n");
        }

        destroy_string(&str);
    }
}

void e_work(FILE* in)
{
    struct String str;

    while (!feof(in))
    {
        str = init_string_from_stream(in, not_endl);

        if (!is_valid_string(&str))
            return;


        destroy_string(&str);
    }
}

void n_work(FILE* in)
{
    struct String str;

    while (!feof(in))
    {
        str = init_string_from_stream(in, not_endl);

        if (!is_valid_string(&str))
            return;


        destroy_string(&str);
    }
}

void s_work(FILE* in)
{
    struct String str;

    while (!feof(in))
    {
        str = init_string_from_stream(in, not_endl);

        if (!is_valid_string(&str))
            return;


        destroy_string(&str);
    }
}

void t_work(FILE* in)
{
    struct String str;

    while (!feof(in))
    {
        str = init_string_from_stream(in, not_endl);

        if (!is_valid_string(&str))
            return;


        destroy_string(&str);
    }
}

int not_endl(int c)
{
    return c != '\n';
}