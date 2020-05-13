/*
**  instructions: this file implements the functions defined
**                in file my_rest.h
**  Author      : Greshem  
**  Create date : 2019-07-11 10:45:01 
**
**   #1 find a file from label_maps 
**   #2 implements find file url function
**   #3 implements the class constructor
**   #4 implements the class member function handle_get_or_post
**
*/

#include "my_rest.h"
//随机返回一个标记文件
std::string find_label_file_core(const std::string key)
{
    /* init random seed */
    //srand((unsigned)time(NULL));
    struct timespec tp;  
    clock_gettime(CLOCK_THREAD_CPUTIME_ID,&tp);  
    srand(tp.tv_nsec);  
    
    /* get the number of tag index */
    DirNode dirNode = label_maps[key];
    
    std::unordered_set<std::string> subdirs = dirNode.get_sub_dirs();
    
    //for(auto itt = subdirs.begin() ; itt != subdirs.end() ; itt++)
    //    std::cout<<key<<" ==== "<<*itt<<std::endl;
    int cnt = subdirs.size();
    log(FILE_NAME,DEBUG,"line %d ==>  find %d subdirs with key %s ",LINE_NUMBER , cnt , key.c_str());
    /*
    *   if pos == cnt then find in current dir
    *       if no files in current dir find in other dir
    *   else
    *       find in other dir;
    */
    std::string url = "";
    //std::cout<<cnt<<" === "<<key<<std::endl;
    if(cnt == 0)
    {
        return dirNode.random_file();
    }    
    int pos = random()%(cnt);

    std::unordered_set<std::string>::iterator it = subdirs.begin();
    for(; it != subdirs.end() && pos ; it ++ , -- pos);

    return find_label_file_core(*it); 
}
/* #2 find a file from label_maps */
std::string find_label_file(const std::string key)
{
    if(key.empty())
    {
        log(FILE_NAME,DEBUG,"line %d ==> KEY is empty or no exist",LINE_NUMBER);
        return "";
    }
   
    int max_time = MAX_FIND_TIMES;
    std::string url = find_label_file_core(key);

    if(url.compare("") == 0)
    {
        url = find_label_file_core(key);
    }
    log(FILE_NAME,DEBUG,"line %d ==>  find resources from label, url ==> %s ",LINE_NUMBER , url.c_str());
    return url;
}
/* #3 implements find file url function*/
std::string find_knowledge_file(const std::string key)
{
    /* tag name validation */
    if(key.empty())
    {
        log(FILE_NAME,DEBUG,"line %d ==> KEY is empty or no exist",LINE_NUMBER);
        return "";
    }
    
    std::string url = "";
    log(FILE_NAME,DEBUG,"line %d ==> %s",LINE_NUMBER,key.c_str());
    //  if cannot find something from label resources
    //  then find from knowledge resources
    if(knowledge_maps.find(key) != knowledge_maps.end())
    {
        url = knowledge_maps[key];
        log(FILE_NAME,DEBUG,"line %d ==> find resources from knowledge",LINE_NUMBER);
    }   
    return url;
}

/* #4 implements the class constructor */
CommandHandler::CommandHandler(utility::string_t url) : m_listener(url)
{
    m_listener.support(methods::GET, std::bind(&CommandHandler::handle_get_or_post, this, std::placeholders::_1));
    m_listener.support(methods::POST, std::bind(&CommandHandler::handle_get_or_post, this, std::placeholders::_1));
}

/* #5 implements the class member function handle_get_or_post */
void CommandHandler::handle_get_or_post(http_request message)
{
    /* debug information */
    log(FILE_NAME,DEBUG,"line %d ==> %s ", LINE_NUMBER , message.method().c_str());
    log(FILE_NAME,DEBUG,"line %d ==> %s ", LINE_NUMBER , http::uri::decode(message.relative_uri().path()).c_str());
    log(FILE_NAME,DEBUG,"line %d ==> %s ", LINE_NUMBER , http::uri::decode(message.relative_uri().query()).c_str());
        
    /* get query parmeters */
    std::map<utility::string_t, utility::string_t>  maps;
    maps = web::uri::split_query(http::uri::decode(message.relative_uri().query()));
    
    /* response value */
    json::value response;
        
    std::string tag_name="";
    if(maps.empty() == true)
    {   
        /* if maps is null or empty  the reply hello */
        file_url = "";
    }
    else
    {   
        tag_name = std::string(maps[U("label")]);
        
        if(tag_name.length() > 0)
        {
            /* avoid attack using long string */
            if(tag_name.length() > MAX_TAG_LEN || tag_name.compare("/label") == 0 || tag_name.compare("/label/") == 0)
            {
                file_url = "";
            }
            else
            {
                /* construct a key */
                std::string tag(tag_name);
                tag.insert(0,"/label/");
                /* find a file url for response*/
                file_url = find_label_file(tag);
            }
        }
        else
        {
            tag_name = std::string(maps[U("name")]);
            if(tag_name.length() > 0 && tag_name.length() <= MAX_TAG_LEN)
            {
                file_url = find_knowledge_file(tag_name);
            }
            else
            {
                file_url = "";
            }
        
        }
    }
    response[U("url")] = json::value::string (file_url);
    response[U("web_base_url")] =json::value::string (web_base_dir);
    response[U("tag")] =json::value::string(tag_name);
    /* reply a request */
    message.reply(status_codes::OK, response);
};
