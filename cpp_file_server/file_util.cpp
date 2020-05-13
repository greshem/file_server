/*
**  instructions: this file implements the functions defined
**                in file_util.h
**  Author      : Greshem  
**  Create date : 2019-07-11 10:45:01 
**
**  #1 init all of the file basic parameters
**  #2 load a resource
**  #3 load all resource needed
*/

#include <time.h>
#include <dirent.h>
#include <fstream>

#include "file_util.h"
#include "log.h"

/* knowledge set*/
std::set<std::string> knowledge_set;

/* label set*/
std::set<std::string> label_set;

/* base directory of target diretory */
std::string base_dir="";

/* base directory of target diretory */
std::string web_base_dir="";

/* ip addr */
std::string ip = DEFAULT_IP;
/* port */
std::string port = DEFAULT_PORT;

/* #1 init all of the file basic parameters */
bool read_config_file(char const * cfgfilepath)  
{  
    std::fstream cfgFile;
    cfgFile.open(cfgfilepath);
    /* judge open status */
    if( ! cfgFile.is_open())
    {
        /* error info */
        log(FILE_NAME , INFO ,"line %d ==> %s ",LINE_NUMBER,"./conf/config.cfg file is not exist!");
        return false;
    }
    
    char tmp[MAX_LEN];
    /* get inotify path and tag name and it's value */
    while(!cfgFile.eof())
    {
        cfgFile.getline(tmp,MAX_LEN);
        std::string line(tmp);
        /* debug info */
        log(FILE_NAME , INFO ,"line %d ==> %s ", LINE_NUMBER , line.c_str());
        
        /* ignore comments line */
        if(tmp[0] == '#')
        {
            continue;
        }
        size_t pos = line.find('=');
        if(pos==std::string::npos)
        {
            log(FILE_NAME , INFO ,"line %d ==> %s ",LINE_NUMBER,"find lines without character \'=\'and ignored");
            continue;
        }
        std::string tmpkey = line.substr(0,pos);
        std::string value = line.substr(pos+1);
        
        if(tmpkey.compare(BASE) == 0)
        {
            /* get base directory */
            base_dir = value;
        }
        else if(tmpkey.compare(IP) == 0)
        {
            /* get rest ip */
            ip = value;
        }
        else if(tmpkey.compare(PORT) == 0)
        {
            /* get rest port */
            port = value;
        }
        else if(tmpkey.compare(WEB_BASE) == 0)
        {
            /* get web base */
            web_base_dir = value;
        }
        else if(tmpkey.compare(KNOWLEDGE) == 0)
        {
            /* get knowledge set */
            knowledge_set.insert(value);
        }
        else if(tmpkey.compare(LABEL) == 0)
        {
            /* get label set */
            label_set.insert(value);
        }
        else if(tmpkey.compare(LOG_LEVEL) == 0)
        {
            /* get label set */
            set_log_level(value);
        }
        else
        {
            log(FILE_NAME , WARNING ,"line %d ==> unhandled line (no useful info): %s ", LINE_NUMBER,line.c_str());
        }
    }
    return true;
}

/* #2 randon get a  file url */
std::string DirNode::random_file()
{
    /*
    *  if no file in this directory
    *  then return empty string( "" )
    */
    if(files.empty())
    {
        return "";
    }
    /* get the amounts of files in this directory */
    int file_num = files.size();
    /* init random seed */
    srand((unsigned)time(NULL));
    
    /* current dir no file exist */
    if(file_num == 0)
    {
        return "";
    }
    
    int index = random() % file_num;
    
    if( index < 0 )
    {
        index = 0 ;
    }
    
    std::unordered_set<std::string>::iterator sit ;
    std::unordered_set<std::string>::iterator sen ;
    
    /*
    *  if(pos > len / 2) find it from the last elment 
    *  else find it from the first element
    */
    
    std::string url;
    
    sit = files.begin();
    sen = files.end();
    while(index)
    {
        -- index;
        ++ sit;
    }
    url = *sit;
    
    
    /*
    **do not need to judge this url
    */
    log(FILE_NAME , INFO ,"line %d ==> %s ",LINE_NUMBER , url.c_str());
    return url;
}

