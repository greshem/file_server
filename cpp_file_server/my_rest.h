/*
**  instructions: this file defined the resource list in memory, and 
**                the operations for updating it.
**  Author      : Greshem  
**  Create date : 2019-07-11 10:45:01 
**
**
*/


#ifndef _MY_REST_H_
#define _MY_REST_H_


#include <stdio.h>
#include "cpprest/uri.h"
#include "cpprest/http_listener.h"
#include "cpprest/asyncrt_utils.h"
#include <pthread.h>

#include "log.h"
#include "inotify_util.h"
#include "file_util.h"

#pragma comment(lib, "cpprest_2_7.lib")
#pragma comment(lib, "bcrypt.lib")
#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "winhttp.lib")
#pragma comment(lib, "httpapi.lib")

using namespace web;
using namespace http;
using namespace utility;
using namespace http::experimental::listener;

/* max tag length */
#define MAX_TAG_LEN     256
#define MAX_FIND_TIMES  5

/* #1 find a file from label maps */
std::string find_label_file_core(const std::string key);

/* #2 find a file from label_maps */
std::string find_label_file(const std::string key);

/* #3 implements find file url function*/
std::string find_knowledge_file(const std::string tag);

/* #4 class commandhandler for handling a requeset and give a response url */
class CommandHandler
{
    public:
        /* constructor of this class */
        CommandHandler() {}
        /* constructor of this class */
        CommandHandler(utility::string_t url);
        /* open a listener */
        inline pplx::task<void> open() 
        {
            return m_listener.open();
        }
        /* close a listener */
        inline pplx::task<void> close()
        {
            return m_listener.close();
        }
    private:
        /* handle a request */
        void handle_get_or_post(http_request message);
        /* a linstener */
        http_listener m_listener;
        /* a response file url */
        std::string file_url = "";
};

#endif
