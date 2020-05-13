/*
**  instructions: this file defined a list of functions of
**                using inotify kernel to monitor files
**  Author      : Greshem
**  Create date : 2019-07-29 16:42:13
**
**
*/

#ifndef _EVENT_HANDLER_H_
#define _EVENT_HANDLER_H_

#include <cstring>
#include <iostream>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <set>
#include <map>
#include <unordered_map>
#include <fstream>
#include <unistd.h>

#include "file_util.h"
#include "log.h"

#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )

/* event need to be watched */
#define WATCH_EVENT (IN_CREATE | IN_DELETE | IN_DELETE_SELF | IN_MODIFY | IN_MOVED_FROM | IN_MOVED_TO)

/* start watch layer*/
#define START_LAYER 0
/*
*   lables maps ==> key: path , value: dir_node class
*/
extern std::unordered_map<std::string, DirNode > label_maps;

/*
*   knowledge_maps ==> key: filename , value: file_url;
*/
extern std::unordered_map<std::string , std::string > knowledge_maps;

/*
*   wd_map ==> key: wd , value: watch_path
*/
extern std::unordered_map<int , std::string> label_wd_map;
extern std::unordered_map<int , std::string> knowledge_wd_map;

/* file amounts */
extern int knowledge_file_amounts;

/* dir amounts */
extern int label_file_amounts;

/* #1 add label inotify core */ 
void add_label_inotify_core(std::string parent_dir , std::string path,  int fd , int layer );

/* #2 add knowledge inotify core */
void add_knowledge_inotify_core(std::string path , int fd , int layer);

/* #3 label event handler */
void label_event_handler(struct inotify_event *event , int fd);

/* #4 knowledge event handler */
void knowledge_event_handler(struct inotify_event *event , int fd);

#endif
