#ifndef PARSER_H
#define PARSER_H

void parse_GPGGA(char *sentence);

// These come from main.c
extern char latitude[12];
extern char fix_status;

#endif
