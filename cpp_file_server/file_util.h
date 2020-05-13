/*
**  instructions: this file defined the resource list in memory, and 
**                the operations for updating it.
**  Author      : Greshem  
**  Create date : 2019-07-11 10:45:01 
**
**
*/

#ifndef _FILE_UTIL_H_
#define _FILE_UTIL_H_

#include <cstring>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <set>
#include <unordered_set>
#include <map>
#include <fstream>
#include <unistd.h>
#include <vector>

/* default ip address */
#define DEFAULT_IP ("127.0.0.1")

/* default port */
#define DEFAULT_PORT ("8080");

/* max character number of config.cfg row */
#define MAX_LEN 500

/* params need to get from config.cfg*/
#define BASE        ("base")
#define IP          ("ip")
#define PORT        ("port")
#define WEB_BASE    ("web_base")
#define KNOWLEDGE   ("knowledge")
#define LABEL       ("label")
#define LOG_LEVEL   ("log_level")

/* base directory of target diretory */
extern std::string base_dir;

/* base directory of target diretory */
extern std::string web_base_dir;

/* knowledge set*/
extern std::set<std::string> knowledge_set;
/* label set*/
extern std::set<std::string> label_set;

/* ip addr */
extern std::string ip;

/* port */
extern std::string port;

/* #1 init basic params */
bool read_config_file(char const* cfgfilepath);

/* #2 dir node class */
class DirNode
{
public:
    DirNode(){}
    DirNode(const DirNode& c_dirNode)
    {
        this->layer = c_dirNode.layer;
        this->current_dir = c_dirNode.current_dir;
        this->parent_dir = c_dirNode.parent_dir;
        this->sub_dirs = c_dirNode.sub_dirs;
        this->files = c_dirNode.files;
    }
    DirNode& operator= (DirNode &c_dirNode)
    {
      
        this->layer = c_dirNode.layer;
        this->current_dir = c_dirNode.current_dir;
        this->parent_dir = c_dirNode.parent_dir;
        this->sub_dirs = c_dirNode.sub_dirs;
        this->files = c_dirNode.files;
       
        return *this;
    }
    /* getter and setter for member layer */
    inline int get_layer()
    {
        return layer;
    }
    inline void set_layer(int lay)
    {
        layer = lay;
    }
    /* getter and setter for member current_dir */
    inline std::string get_current_dir()
    {
        return current_dir;
    }
    inline void set_current_dir(std::string dir)
    {
        current_dir = dir;
    }
    /* getter and setter for member parent_dir */
    inline std::string get_parent_dir()
    {
        return parent_dir;
    }
    inline void set_parent_dir(std::string dir)
    {
        parent_dir = dir;
    }
    /* adder and deleter for member sub_dirs */
    inline void add_subdir(std::string subdir)
    {
        sub_dirs.insert(subdir);
    }
    inline int delete_subdir(std::string subdir)
    {
        return sub_dirs.erase(subdir);
    }
    /* adder and deleter for member files */
    inline void add_file(std::string file)
    {
        files.insert(file);
    }
    inline int delete_file(std::string file)
    {
        return files.erase(file);
    }
    /* getter for member subdirs and files*/
    inline std::unordered_set<std::string> get_sub_dirs()
    {
        return sub_dirs;
    }
    inline std::unordered_set<std::string> get_files()
    {
        return files;
    }
    /* setter for member subdirs and files*/
    inline void set_sub_dirs(std::unordered_set<std::string> subdirs)
    {
        sub_dirs = subdirs;
    }
    inline void set_files(std::unordered_set<std::string> file)
    {
        files = file;
    }
    
    /* get a file from current_dir randomly */
    std::string random_file();

private:
    /* layer */
    int layer = 0;
    /* current dir*/
    std::string current_dir = "";
    
    /* parent dir */
    std::string parent_dir = "";
    
    /* the sub-dirs in current dir*/
    std::unordered_set<std::string> sub_dirs;
    
    /* the files in current dir */
    std::unordered_set<std::string> files;
};

#endif

