/*
**  Description:  log.cpp implements a simple log writer
**  Author     :  Greshem
**  Datetime   :  2019-07-29 13:49:09
**
**  #1 current log level
**  #2 set log file level
**  #3 construct filename
**  #4 output info to logfile 
*/

#include "log.h"

/* #1 current log level */
unsigned int g_current_log_level = ERROR;

/* #1.1 logfilename */
char log_file_name[] = LOGFILE;

/* #2 set log file level */
int set_log_level(std::string str)
{
    int log_level = -1;
    if(str.compare("INFO") == 0)
    {
        /* set level  to INFO */
        log_level = INFO;
    }
    else if(str.compare("DEBUG") == 0)
    {
        /* set level  to DEBUG */
        log_level = DEBUG;
    }
    else if(str.compare("WARNING") == 0)
    {
        /* set level  to WARNING */
        log_level = WARNING;
    }
    else if(str.compare("ERROR") == 0)
    {
        /* set level  to ERROR */
        log_level = ERROR;
    }
    else if(str.compare("FATAL") == 0)
    {
        /* set level  to FATAL */
        log_level = FATAL;
    }
    else
    {
        /* set level  to FATAL+1 */
        log_level = FATAL + 1;
    }
    /* validation input log_level */
    if(log_level > 4 || log_level <0 )
    {
        return -1;
    }
    g_current_log_level = log_level;
    return 0;
}

/* #3 construct filename */
void getLogFileName(char const* filename , char * logfile)
{
    struct tm *mytm;
    FILE *fp;
    time_t myt;
    char date[20] = "";

    time(&myt);
    mytm = localtime(&myt);
    strftime(date, 20, "%Y-%m-%d", mytm);
    
    strcat(logfile,filename);
    strcat(logfile,"-");
    strcat(logfile,date);
    strcat(logfile,".log");
    
}

/* #4 output info to logfile */
int log(std::string filename ,unsigned int log_level , std::string str, ...)
{
    /* check log level */
    if(log_level < g_current_log_level)
    {
        return 0;
    }
    
    struct tm *mytm;
    FILE *fp;
    time_t myt;
    char t_time[20] = "";
    char t_day[20] = "";
    char logfile[1024] = "";
    char log[1024] = "";
    char ch;
    char c;
    int d;
    char *s;
    time(&myt);
    mytm = localtime(&myt);
    /* get current time */
    strftime(t_time, 20, "%H:%M:%S", mytm);
    
    getLogFileName(log_file_name , logfile);
    
    OPENFILE(fp, logfile , "a+");
    
    /* output level to log file */
    switch(log_level)
    {
        case INFO : fprintf(fp,"%s","INFO : ");
                    break;
        case DEBUG : fprintf(fp,"%s","DEBUG : ");
                     break;
        case WARNING : fprintf(fp,"%s","WARNING : ");
                       break;
        case ERROR : fprintf(fp,"%s","ERROR : ");
                     break;
        case FATAL : fprintf(fp,"%s","FATAL : ");
                     break;
        default:
            break;
    }
    
    strncpy(log, t_time , strlen(t_time));
    strcat(log,"--");
    strncat(log, filename.c_str() , strlen(filename.c_str()));
    strcat(log," -> ");
    fprintf(fp, "%s", log);
    
    va_list ap;
    va_start(ap, &str);
    
    int i = 0;
    for(; i < str.length() ; i++)
    {
        ch = str[i];
        //putchar(ch);
        if(ch != '%')
        {
            fprintf(fp, "%c", ch);
            continue;
        }
        
        switch(++i,str[i])
        {
        case 'd':
            d = va_arg(ap, int);
            fprintf(fp, "%d", d);
            break;
        case 'c':
            c = va_arg(ap, int);
            fprintf(fp, "%c", c);
            break;
        case 's':
            s = va_arg(ap, char *);
            fprintf(fp, "%s", s);
            break;
        default:
            break;
        }
    }
    va_end(ap);
    fputs("\n" , fp);
    fclose(fp);
    return 0;
}


