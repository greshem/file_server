/*
**  Description:  log.h define a simple log writer
**  Author     :  Greshem
**  Datetime   :  2019-07-29 14:25:29
*/

#ifndef _LOG_H
#define _LOG_H

#include<iostream>
#include<cstring>
#include<stdio.h>
#include<string.h>
#include<time.h>
#include<stdarg.h>
#include<unistd.h>

/* log level */
#define INFO 0
#define DEBUG 1
#define WARNING 2
#define ERROR 3
#define FATAL 4

#define FILE_NAME    (__FILE__)
#define LINE_NUMBER  (__LINE__)

#define OPENFILE(fp, filename, type) \
    if((fp=fopen(filename, type)) == NULL) \
    {printf("can not open the file %s\n", filename);return -1;}

#define LOGFILE ("./log/fileservice")

/* current log level */
extern unsigned int g_current_log_level;

/* set log file level */
int set_log_level(std::string str);

/* get log file name with date */
void getLogFileName(char const *filename , char * logfile);

/* output info to log file */
int log(std::string filename , unsigned int loglevel , std::string str, ...);

#endif
