#ifndef PARSER_H
#define PARSER_H

void parse_GPRMC(char *sentence);

extern char utc_time[18];
extern char latitude[16];
extern char longitude[16];
extern char fix_status;

#endif
