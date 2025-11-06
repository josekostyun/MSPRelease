#ifndef PARSER_H
#define PARSER_H

void parse_GPRMC(char *sentence);

extern char utc_time[12];
extern char latitude[12];
extern char longitude[12];
extern char fix_status;

#endif
