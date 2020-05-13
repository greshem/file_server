/*
**  instructions: this file defined the resource list in memory, and 
**                the operations for updating it.
**  Author      : Greshem  
**  Create date : 2019-07-11 10:45:01 
**  
**  #1 entrance of server start
*/
#include <stdio.h>

#include "file_util.h"
#include "inotify_util.h"
#include "log.h"
#include "my_rest.h"

#define CONFIG_FILE ("./conf/config.cfg")
#define SUCCESS 0
#define T_ERROR -1

/* #1 entrance of server start*/
int main(int argc, char **argv)
{
    /* init basis information */
    read_config_file(CONFIG_FILE);
    
    /* start a inotify thread to watch file changes in a path*/
    pthread_t t_label , t_knowledge;
    
    int ret = pthread_create(&t_label,NULL,&label_core,NULL);
    
    if(ret != 0)
    {
        log(FILE_NAME,ERROR,"line %d ==> start label inotify thread failed",LINE_NUMBER);
        return T_ERROR;
    }
    
    ret = pthread_create(&t_knowledge,NULL,&konwledge_core,NULL);
    
    if(ret != 0)
    {
        log(FILE_NAME,ERROR,"line %d ==> start knowledge inotify thread failed",LINE_NUMBER);
        return T_ERROR;
    }
    
    try
    {   
        /* start  a rest server */
        std::string s_addr = "http://";
        s_addr.append(ip);
        s_addr.append(":").append(port).append("/resource");
        log(FILE_NAME,DEBUG,"line %d ==> web server start at: %s ",LINE_NUMBER , s_addr.c_str());
        utility::string_t address = U(s_addr);
        uri_builder uri(address);
        auto addr = uri.to_uri().to_string();
        CommandHandler handler(addr);
        handler.open().wait();
        
        /* debug information */
        if(true)
        {
            ucout << utility::string_t(U("Listening for requests at: ")) << addr << std::endl;
            ucout << U("Press ENTER key to quit...") << std::endl;
            std::string line;
            std::getline(std::cin, line);
            handler.close().wait();
            log(FILE_NAME,DEBUG,"line %d ==> web server end",LINE_NUMBER);
        }
     }
    catch (std::exception& ex)
    {
        /*
        ucout << U("Exception: ") << ex.what() << std::endl;
        ucout << U("Press ENTER key to quit...") << std::endl;
        std::string line;
        std::getline(std::cin, line);
        */
        log(FILE_NAME,ERROR,"line %d ==> web server end with exception: %s",LINE_NUMBER,ex.what());
    }
    return SUCCESS;
}
