/*
**  instructions: this file defined a list of functions of
**                using inotify kernel to monitor files
**  Author      : Greshem
**  Create date : 2019-07-29 16:42:13
**
**  #1 init inotify
**  #2 add watch 
**  #3 add label inotify
**  #4 add knowledge inotify
**  #5 rm inotify
**  #6 knowledge core
**  #7 label core
*/

#include "inotify_util.h"

/* #1 init inotify */
int init_inotify()
{
    
    int fd = inotify_init();
    
    if ( fd < 0 )
    {
        log(FILE_NAME , ERROR ,"line %d ==> %s ", LINE_NUMBER , "inotify fd init failed");
        return -1;
    }
    
    return fd;
}

/* #2 add watch */
bool add_watch(std::string path , int fd , bool flag)
{
    /* construct absolutely path */
    std::string full_path(base_dir);
    full_path.append(path);
    
    int wd = inotify_add_watch(fd, full_path.c_str() , WATCH_EVENT);
    /*
    * if wd < 0 ==> watch failed return false;
    * else return true;
    */
    if(wd < 0)
    {
        log(FILE_NAME , ERROR ,"line %d ==> add watch path %s has failed",LINE_NUMBER , path.c_str());
        return false;
    }
    /*
    *   flag: true  ==>  label
    *   flag: false ==> knowledge
    */
    if(flag == true)
    {
        label_wd_map[wd] = path;
    }
    else
    {
        knowledge_wd_map[wd] = path;
    }
    return true;
}

/* #3 add label inotify */ 
bool add_label_inotify(std::set<std::string> &label_set,  int fd)
{
    if(label_set.empty())
    {
        log(FILE_NAME , WARNING ,"line %d ==> No lable resource need to be monitored!",LINE_NUMBER );
        return false;
    }
    log(FILE_NAME , WARNING ,"line %d ==> Load Resources BEGIN(label) and current file amounts: %d " , LINE_NUMBER,label_file_amounts);
    for(std::set<std::string>::iterator it = label_set.begin() ; it != label_set.end() ; it++)
    {
        add_label_inotify_core("" , *it , fd , START_LAYER);
    }
    log(FILE_NAME , WARNING ,"line %d ==> Load Resources END(label) and current file amounts: %d " , LINE_NUMBER,label_file_amounts);
    return true;
}

/* #4 add knowledge inotify */ 
bool add_knowledge_inotify(std::set<std::string> &knowledge_set, int fd)
{
    if(knowledge_set.empty())
    {
        log(FILE_NAME , WARNING ,"line %d ==> No knowledge resource need to be monitored!" , LINE_NUMBER);
        return false;
    }
    
    log(FILE_NAME , WARNING ,"line %d ==> Load Resources BEGIN(knowledge) and current file amounts: %d " , LINE_NUMBER,knowledge_file_amounts);
    for(std::set<std::string>::iterator it = knowledge_set.begin() ; it != knowledge_set.end() ; it++)
    {
        add_knowledge_inotify_core(*it , fd , START_LAYER);
    }
    log(FILE_NAME , WARNING ,"line %d ==> Load Resources END(knowledge) and current file amounts: %d " , LINE_NUMBER,knowledge_file_amounts);
    return true;
}

/* #5 rm inotify */
void rm_inotify(int fd , std::unordered_map<int , std::string> &maps)
{
    for(std::unordered_map<int , std::string>::iterator it = maps.begin() ; it != maps.end() ; it ++)
    {
        (void) inotify_rm_watch(fd,it->first);
    }
    
    /* clear all the elements in maps */
    maps.clear();
}

/* #6 knowledge core */
void* konwledge_core(void *)
{
    log(FILE_NAME , INFO ,"line %d ==> knowledge inotify thread start" , LINE_NUMBER);
    /* init fd */
    int fd = init_inotify();
    /* init fd failed*/
    if(fd == -1)
    {
        log(FILE_NAME , INFO ,"line %d ==> knowledge inotify thread end cannot init fd" , LINE_NUMBER);
        return (void*)NULL;
    }
    
    add_knowledge_inotify(knowledge_set , fd);
    
    char buffer[BUF_LEN];
    
    int length = 0;
    while((length = read( fd, buffer, BUF_LEN )) > 0)
    {
        log(FILE_NAME , INFO ,"line %d ==> knowledge inotify event happened" , LINE_NUMBER);
        int i = 0 ;
        while ( i < length )
        {
            struct inotify_event *event = ( struct inotify_event * ) &buffer[ i ];
            if ( event->len )
            {
                knowledge_event_handler(event,fd);
            }
            i += EVENT_SIZE + event->len;
        }
    }
    rm_inotify( fd , label_wd_map);
    ( void ) close( fd );
    
}
/* #7 label core */
void* label_core(void *)
{
    log(FILE_NAME , INFO ,"line %d ==> label inotify thread start" , LINE_NUMBER);
    /* init fd */
    int fd = init_inotify();
    /* init fd failed*/
    if(fd == -1)
    {
        log(FILE_NAME , INFO ,"line %d ==> label inotify thread end cannot init fd" , LINE_NUMBER);
        return (void*)NULL;
    }
    
    add_label_inotify(label_set , fd);
    
    char buffer[BUF_LEN];
    
    int length = 0;
    while((length = read( fd, buffer, BUF_LEN )) > 0)
    {
        log(FILE_NAME , INFO ,"line %d ==> label inotify event happened" , LINE_NUMBER);
        int i = 0 ;
        while ( i < length )
        {
            struct inotify_event *event = ( struct inotify_event * ) &buffer[ i ];
            if ( event->len )
            {
                label_event_handler(event,fd);
            }
            i += EVENT_SIZE + event->len;
        }
    }
    rm_inotify( fd , knowledge_wd_map );
    ( void ) close( fd );
}

