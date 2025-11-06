#include <string.h>
#include "parser.h"

void parse_GPGGA(char *sentence)
{
    char *ptr = sentence;
    int field = 0;
    int idx = 0;

    while(*ptr)
    {
        if(*ptr == ',')
        {
            field++;
            idx = 0;
            ptr++;
            continue;
        }

        if(field == 2 && idx < 11)
        {
            latitude[idx++] = *ptr;
            latitude[idx] = '\0';
        }
        else if(field == 6)
        {
            fix_status = *ptr;
        }

        ptr++;
    }
}
