/*
**  instructions: this file defined a list of functions of
**                using inotify kernel to monitor files
**  Author      : Greshem
**  Create date : 2019-07-29 16:42:13
**
**
*/

#ifndef _INOTIFY_UTIL_H_
#define _INOTIFY_UTIL_H_

#include "event_handler.h"

/* #1 init inotify */
int init_inotify();

/* #2 add watch*/
bool add_watch(std::string path , int fd , bool flag );

/* #3 add label inotify */ 
bool add_label_inotify(std::set<std::string> &label_set,  int fd );

/* #4 add knowledge inotify */ 
bool add_knowledge_inotify(std::set<std::string> &knowledge_set, int fd);

/* #5 rm inotify */
void rm_inotify(int fd , std::unordered_map<int , std::string > &maps);

/* #6 knowledge core */
void* konwledge_core(void *);

/* #7 label core */
void* label_core(void*);

#endif
