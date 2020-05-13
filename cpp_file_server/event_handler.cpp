/*
**  instructions: this file implements the functions defined
**                in file event_handler.h
**  Author      : Greshem  
**  Create date : 2019-08-01 15:36:31 2019-07-11
**
**  #1 add label inotify core
**  #2 label event handler
**  #3 add knowledge inotify core
**  #4 knowledge event handler
*/

#include "event_handler.h"
/*
*   lables maps ==> key: path , value: DirNode class
*/
std::unordered_map<std::string, DirNode > label_maps;

/*
*   knowledge_maps ==> key: filename , value: file_url;
*/
std::unordered_map<std::string , std::string > knowledge_maps;

/*
*   wd_map ==> key: wd , value: watch_path
*/
std::unordered_map<int , std::string> label_wd_map;
std::unordered_map<int , std::string> knowledge_wd_map;


/* file amounts */
int label_file_amounts = 0;

/* dir amounts */
int knowledge_file_amounts = 0;

/* #1 add label inotify core */
void add_label_inotify_core(std::string parent_dir , std::string path , int fd , int layer)
{
    DIR *dir;
    DirNode dirNode;
    struct dirent *ptr;
    /* constructor full path to inotify */
    std::string full_path(base_dir);
    full_path.append(path);
    //printf("%s aaaa\n",full_path.c_str());
    /* debug load resource full path */
    log(FILE_NAME,DEBUG,"line %d ==> full path(label): %s",LINE_NUMBER , full_path.c_str());
    if ((dir=opendir(full_path.c_str())) == NULL)
    {
        log(FILE_NAME,ERROR,"line %d ==> Directory open error(label): %s",LINE_NUMBER,full_path.c_str());
        return;
    }
    
    /* add watch event for fd */
    int wds = inotify_add_watch( fd, full_path.c_str(), WATCH_EVENT );
    if(wds < 0)
    {
        //watch out failed
        log(FILE_NAME,ERROR,"line %d ==> Path: %s watch failed(label)!",LINE_NUMBER,full_path.c_str());
        return;
    }
    /* add <wds , new_dir > to label_wd_map */
    label_wd_map[wds] = path;
    
    /* add members value to dir node */
    dirNode.set_current_dir(path);
    dirNode.set_parent_dir(parent_dir);
    dirNode.set_layer(layer);
    
    while ((ptr=readdir(dir)) != NULL)
    {
        log(FILE_NAME , INFO , "line %d ==> load_resource_info(label) %d",LINE_NUMBER , ptr -> d_type);
        if(strncmp(ptr->d_name,".",1)==0 || strncmp(ptr->d_name,"..",2)==0)
        {
            /* current dir OR parrent dir */
            continue;
        }
        else if(ptr->d_type == DT_REG)
        {
            /* file */
            label_file_amounts ++;
            
            std::string temp(path);
            std::string file_path = temp.append("/").append(ptr->d_name);
            log(FILE_NAME , INFO , "line %d ==> file relative path(label): %s " , LINE_NUMBER , file_path.c_str());
            dirNode.add_file(file_path);
        }
        else if(ptr->d_type == DT_DIR )
        {
            /* directory */
            label_file_amounts ++;
            
            std::string temp(path);
            std::string base = temp.append("/").append(ptr->d_name);
            
            log(FILE_NAME , INFO , "line %d ==> sub-dir relative path(label): %s ",LINE_NUMBER , base.c_str());
            
            /* add sub dir to dirNode */
            dirNode.add_subdir(base);
            
            /* load resources from sub dir*/
            add_label_inotify_core(path , base , fd , layer+1);
        }
        else if(ptr->d_type == DT_UNKNOWN)
        {
            log(FILE_NAME , WARNING , "line %d ==> cannot recognize the file type(label)!",LINE_NUMBER);
            std::string temp(full_path);
            std::string tmp_dir = temp.append("/").append(ptr->d_name);
            DIR *tmp_ptr_dir;
            if((tmp_ptr_dir=opendir(tmp_dir.c_str())) == NULL)
            {
                 /* file */
                 label_file_amounts ++;
                 
                 std::string temp(path);
                 std::string file_path = temp.append("/").append(ptr->d_name);
                 log(FILE_NAME , WARNING , "line %d ==> file relative path(label): %s",LINE_NUMBER , file_path.c_str());
                 dirNode.add_file(file_path);
            }
            else
            {
                /* directory */
                label_file_amounts ++;
                std::string temp(path);
                std::string base = temp.append("/").append(ptr->d_name);
                
                log(FILE_NAME , WARNING , "line %d ==> sub-dir relative path(label): %s ", LINE_NUMBER , base.c_str());
                /* add sub dir to dirNode */
                dirNode.add_subdir(base);
                
                /* load resources from sub dir*/
                add_label_inotify_core(path , base , fd , layer+1);
            }
         }
     }
     
     /* add label value to maps */
     label_maps[path] = dirNode;
     
     closedir(dir);
}

/* #2 label event handler */
void label_event_handler(struct inotify_event *event , int fd)
{
 // event is empty
    if(event == NULL)
    {
        return;
    }
    // do not handle hiden files
    if(event->name[0] == '.')
    {
        log(FILE_NAME , DEBUG , "line %d ==> find hiden file(label)%s " , LINE_NUMBER , event->name);
        return;
    }
    /* get current path and resource tag index */ 
    int wd = event->wd;
    
    /*
    *  if can not find element with key==wd
    *  then return with a fatal error info ;
    */
    //if(label_wd_map.find(wd) == label_wd_map.end())
    //{
    //    log(FILE_NAME , FATAL , "line %d ==> event handler happened but does not in label_wd_map",LINE_NUMBER);
    //    return ;
    //}
    
    std::string current_dir(label_wd_map[wd]);
    
    /* get new path */
    std::string temp(current_dir);
    std::string new_dir = temp.append("/").append(event->name);
    
    log(FILE_NAME , DEBUG , "line %d ==> current dir(label): %s" , LINE_NUMBER , current_dir.c_str());
    log(FILE_NAME , DEBUG , "line %d ==> current event path(label): %s" , LINE_NUMBER , new_dir.c_str());
    
    if ( event->mask & IN_CREATE ) 
    {// create event
        if ( event->mask & IN_ISDIR ) 
        {   
            /* watch this sub-dir */
            std::string full_path(base_dir);
            full_path.append(new_dir);
            int wds = -1;
            /* times for try to watch this dir*/
            int times = 3;
            while(wds < 0 && times > 0)
            {
                times --;
                wds = inotify_add_watch( fd, full_path.c_str(), WATCH_EVENT );
                //watch out failed
                log(FILE_NAME , ERROR , "line %d ==> watch failed for created directory(label): try %d times for dir %s ",LINE_NUMBER , 3 - times , new_dir.c_str());
            }
            /* watch failed */
            if(wds < 0)
            {
                return ;
            }
            /* add <wds , new_dir > to label_wd_map */
            label_wd_map[wds] = new_dir;
            // create a dir , watch it
            log(FILE_NAME , DEBUG , "line %d ==> create directory and watch SUCCESS (label)=> wd: %d dir: %s",LINE_NUMBER , wds , new_dir.c_str());
            
            /* add new dir to label_maps */
            label_maps[current_dir].add_subdir(new_dir);
            
        }
        else
        {// create a file
            log(FILE_NAME , DEBUG , "line %d ==> create file(label): %s", LINE_NUMBER , event->name);
            label_maps[current_dir].add_file(new_dir);
        }
    }
    else if ( event->mask & IN_DELETE )
    {// delete event
        if ( event->mask & IN_ISDIR ) 
        {// delete a dir , delete watch it
            
            /* delete wd from wd_map , the file in the directory delete already */
            //int ans = label_wd_map.erase(wd);

            log(FILE_NAME , DEBUG , "line %d ==> SUCCESS todelete directory(label): %s", LINE_NUMBER , event->name);

        }
        else 
        {// delete a file
            int ans = label_maps[current_dir].delete_file(new_dir);
            if(ans == 1)
            {
                log(FILE_NAME , DEBUG , "line %d ==> SUCCESS to delete file(label): %s", LINE_NUMBER , event->name);
            }
            else
            {
                log(FILE_NAME , ERROR , "line %d ==> FAILED to delete file(no exist (label)): %s", LINE_NUMBER , event->name);
            }
        }
    }
    else if ( event->mask & IN_MODIFY ) 
    {
        if ( event->mask & IN_ISDIR ) 
        {// modify a dir
            log(FILE_NAME , DEBUG , "line %d ==> modify directory(label): %s", LINE_NUMBER , event->name);
            /* add sub dir to current dir*/
            label_maps[current_dir].add_subdir(new_dir);
            /*add files in this dir and subdirs */
            add_label_inotify_core(current_dir, new_dir, fd , label_maps[current_dir].get_layer() + 1 );
        }
        else
        {// modify a file
            //p->file_modify();
            log(FILE_NAME , DEBUG , "line %d ==> modify file(label): %s",LINE_NUMBER , event->name);
            label_maps[current_dir].add_file(new_dir);
        }
    }
    else if ( event->mask & IN_DELETE_SELF ) 
    {
        if ( event->mask & IN_ISDIR ) 
        {// in delete a dir
            log(FILE_NAME , DEBUG , "line %d ==> delete directory self(label): %s", LINE_NUMBER , event->name);
            //inotify_rm_watch( fd, wd );
            //p->dir_delete(new_dir, index);
        }
        else
        {// in delete a file
            int ans = label_maps[current_dir].delete_file(new_dir);
            if(ans == 1)
            {
                log(FILE_NAME , DEBUG , "line %d ==> SUCCESS to delete file self(label): %s" , LINE_NUMBER , event->name);
            }
            else
            {
                log(FILE_NAME , DEBUG , "line %d ==> FAILED to delete file self(no exist (label)): %s", LINE_NUMBER , event->name);
            }
        }
    }
    else if ( event->mask & IN_MOVED_FROM ) 
    {//
        if ( event->mask & IN_ISDIR ) 
        {// moved from a dir
            log(FILE_NAME , DEBUG , "line %d ==> in moved from directory(label): %s" , LINE_NUMBER , event->name);
            /* remove this dir from DirNode->sub_dirs from parent dir*/
            label_maps[current_dir].delete_subdir(new_dir);
            /* erase new dir from label_maps */
            label_maps.erase(new_dir);
        }
        else
        {// moved from a file
            log(FILE_NAME , DEBUG , "line %d ==> file in moved from (label): %s" , LINE_NUMBER , event->name);
            label_maps[current_dir].delete_file(new_dir);
        }
    }
    else if ( event->mask & IN_MOVED_TO ) 
    {
        if ( event->mask & IN_ISDIR ) 
        {// moved to a dir
            log(FILE_NAME , ERROR , "line %d ==> in moved to a directory(label): %s", LINE_NUMBER , event->name);
            /* add sub dir to current dir*/
            label_maps[current_dir].add_subdir(new_dir);
            /*add files in this dir and subdirs */
            add_label_inotify_core(current_dir, new_dir, fd , label_maps[current_dir].get_layer() + 1 );
        }
        else
        {// moved to a file
            log(FILE_NAME , INFO , "line %d ==> in moved to a file(label): %s", LINE_NUMBER , event->name);
            label_maps[current_dir].add_file(new_dir);
        }
    }
    
    log(FILE_NAME , INFO , "line %d ==>label_wd_map: ",LINE_NUMBER);
    std::unordered_map<int , std::string>::iterator its1 = label_wd_map.begin();
    std::unordered_map<int , std::string>::iterator ite1 = label_wd_map.end();
    for(; its1!= ite1 ; its1++)
    {
        log(FILE_NAME , INFO , "line %d ==> label_wd ==> %d , label_dir ==> %s", LINE_NUMBER , its1->first,(its1->second).c_str());
    }
    
    log(FILE_NAME , INFO , "line %d ==> label_maps: " , LINE_NUMBER);
    std::unordered_map<std::string, DirNode>::iterator its2 = label_maps.begin();
    std::unordered_map<std::string, DirNode>::iterator ite2 = label_maps.end();
    for(; its2!= ite2 ; its2 ++)
    {
        DirNode dn = its2->second;
        int layer = dn.get_layer();
        std::string p_dir = dn.get_parent_dir();
        std::string c_dir = dn.get_current_dir();
        std::unordered_set<std::string> sdirs = dn.get_sub_dirs();
        std::unordered_set<std::string> fs = dn.get_files();
        log(FILE_NAME , INFO , "line %d ==> key: %s " , LINE_NUMBER , (its2->first).c_str());
        log(FILE_NAME , INFO , "line %d ==> layer: %d ", LINE_NUMBER , layer);
        log(FILE_NAME , INFO , "line %d ==> parent_dir: %s", LINE_NUMBER , p_dir.c_str());
        log(FILE_NAME , INFO , "line %d ==> c_dir= %s", LINE_NUMBER , c_dir.c_str());
        for(std::unordered_set<std::string>::iterator its3 = sdirs.begin() ; its3 != sdirs.end() ; its3 ++)
        {
            log(FILE_NAME , INFO , "line %d ==> subdir: %s", LINE_NUMBER , (*its3).c_str());
        }
        for(std::unordered_set<std::string>::iterator its4 = fs.begin() ; its4 != fs.end() ; its4 ++)
        {
            log(FILE_NAME , INFO , "line %d ==> file: %s", LINE_NUMBER , (*its4).c_str());
        }
    }
}

/* #3 add knowledge inotify core */
void add_knowledge_inotify_core(std::string path , int fd , int layer)
{
     DIR *dir;
     struct dirent *ptr;
     /* constructor full path to inotify */
     std::string full_path(base_dir);
     full_path.append(path);
     /* debug load resource full path */
     log(FILE_NAME , DEBUG , "line %d ==> full path(knowledge): %s", LINE_NUMBER , full_path.c_str());
     if ((dir=opendir(full_path.c_str())) == NULL)
     {
         log(FILE_NAME , DEBUG , "line %d ==> Directory open error(knowledge) %s ", LINE_NUMBER , full_path.c_str());
         return;
     }
     
     /* add watch dir to fd*/
     int wds = inotify_add_watch( fd, full_path.c_str(), WATCH_EVENT );
     if(wds < 0)
     {
        //watch out failed
        log(FILE_NAME , ERROR , "line %d ==>watch failed(knowledge)!" , LINE_NUMBER);
        return;
     }
     /* add <wds , new_dir > to label_wd_map */
     knowledge_wd_map[wds] = path;
     
     while ((ptr=readdir(dir)) != NULL)
     {
         log(FILE_NAME , DEBUG , "line %d ==> file type ==> %d", LINE_NUMBER ,ptr->d_type);
         if(strncmp(ptr->d_name,".",1)==0 || strncmp(ptr->d_name,"..",2)==0)
         {
             /* current dir OR parrent dir */
             continue;
         }
         else if(ptr->d_type == DT_REG)
         {
             /* file */
             std::string temp(path);
             std::string file_path = temp.append("/").append(ptr->d_name);
             std::string keytemp =ptr->d_name;
             
             knowledge_file_amounts ++;
             size_t pos = keytemp.find_last_of('.');
             std::string key = keytemp.substr(0,pos);
             
             std::unordered_map<std::string , std::string>::iterator it = knowledge_maps.find(key);
             if(it != knowledge_maps.end())
             {
                log(FILE_NAME , WARNING , "line %d ==> create a file and the key already exist.Overcover key: %s value: %s (knowledge)" , LINE_NUMBER , (it->first).c_str() , (it->second).c_str() );
             }
             log(FILE_NAME , DEBUG , "line %d ==> file relative path(knowledge): %s", LINE_NUMBER , file_path.c_str());
             knowledge_maps[key] = file_path;
             
         }else if(ptr->d_type == DT_DIR )
         {
             /* directory */
             knowledge_file_amounts ++;
             std::string temp(path);
             std::string base = temp.append("/").append(ptr->d_name);
             
             log(FILE_NAME , DEBUG , "line %d ==> sub-dir relative path(knowledge): %s ", LINE_NUMBER , base.c_str());
             
             /* load resources from sub dir*/
             add_knowledge_inotify_core(base , fd , layer+1);
         }
         else if(ptr->d_type == DT_UNKNOWN)
         {
             log(FILE_NAME , WARNING , "line %d ==> cannot recognize file type(knowledge) ", LINE_NUMBER);
             std::string temp(full_path);
             std::string tmp_dir = temp.append("/").append(ptr->d_name);
             DIR *tmp_ptr_dir;
             if((tmp_ptr_dir=opendir(tmp_dir.c_str())) == NULL)
             {
                /* file */
                knowledge_file_amounts ++;
                std::string temp(path);
                std::string file_path = temp.append("/").append(ptr->d_name);
                
                std::string keytemp =ptr->d_name;
                size_t pos = keytemp.find_last_of('.');
                std::string key = keytemp.substr(0,pos);
                
                std::unordered_map<std::string , std::string>::iterator it = knowledge_maps.find(key);
                if(it != knowledge_maps.end())
                {
                    log(FILE_NAME , WARNING , "line %d ==> create a file and the key already exist.Overcover key: %s value: %s (knowledge)" , LINE_NUMBER , (it->first).c_str() , (it->second).c_str() );
                }
                
                log(FILE_NAME , INFO , "line %d ==> file relative path(knowledge): %s", LINE_NUMBER , file_path.c_str());
                knowledge_maps[key] = file_path;
             }
             else
             {
                /* directory */
                knowledge_file_amounts ++;
                std::string temp(path);
                std::string base = temp.append("/").append(ptr->d_name);
                
                log(FILE_NAME , INFO , "line %d ==> sub-dir relative path(knowledge): %s ", LINE_NUMBER , base.c_str());
                
                /* load resources from sub dir*/
                add_knowledge_inotify_core(base , fd , layer+1);
            }
         }
     }
     closedir(dir);
}

/* #4 knowledge event handler */
void knowledge_event_handler(struct inotify_event *event , int fd)
{
    /* event is empty */
    if(event == NULL)
    {
        return;
    }
    /* do not handle hiden files */
    if(event->name[0] == '.')
    {
        log(FILE_NAME , INFO , "line %d ==> find hiden file %s and ignored(knowledge)", LINE_NUMBER , event->name);
        return;
    }
    /* get current path and resource tag index */ 
    int wd = event->wd;
    
    /*
    *  if can not find element with key==wd
    *  then return with a fatal error info ;
    */
    //if(knowledge_wd_map.find(wd) == knowledge_wd_map.end())
    //{
    //    log(FILE_NAME , FATAL , "line %d ==> event handler happened but does not in knowledge_wd_map", LINE_NUMBER);
    //    return ;
    //}
    
    std::string current_dir(knowledge_wd_map[wd]);
    
    /* get new path */
    std::string temp(current_dir);
    std::string new_dir = temp.append("/").append(event->name);
    
    log(FILE_NAME , DEBUG , "line %d ==> current dir(knowledge): %s", LINE_NUMBER , current_dir.c_str());
    log(FILE_NAME , DEBUG , "line %d ==> current event file path(knowledge): %s", LINE_NUMBER , new_dir.c_str());
    
    if ( event->mask & IN_CREATE ) 
    {// create event
        if ( event->mask & IN_ISDIR ) 
        {// create a dir , watch it
            log(FILE_NAME , INFO , "line %d ==> create directory(knowledge): %s",LINE_NUMBER , event->name);
            /* watch this sub-dir */
            std::string full_path(base_dir);
            full_path.append(new_dir);
            int wds = inotify_add_watch( fd, full_path.c_str(), WATCH_EVENT );
            if(wds < 0)
            {
                //watch out failed
                log(FILE_NAME , ERROR , "line %d ==> watch failed for created directory(knowledge): %s",LINE_NUMBER,event->name);
                return;
            }
            /* add <wds , new_dir > to label_wd_map */
            knowledge_wd_map[wds] = new_dir;
        }
        else
        {// create a file
            log(FILE_NAME , INFO , "line %d ==> create file(knowledge): %s",LINE_NUMBER,event->name);
            
            std::string keytemp =event->name;
            size_t pos = keytemp.find_last_of('.');
            std::string key = keytemp.substr(0,pos);
            std::unordered_map<std::string , std::string>::iterator it = knowledge_maps.find(key);
            if(it != knowledge_maps.end())
            {
                log(FILE_NAME , WARNING , "line %d ==> create a file and the key already exist.Overcover key: %s value: %s (knowledge)" , LINE_NUMBER , (it->first).c_str() , (it->second).c_str() );
            }
            
            knowledge_maps[key] = new_dir;
        }
    }
    else if ( event->mask & IN_DELETE )
    {// delete event
        if ( event->mask & IN_ISDIR ) 
        {// delete a dir , delete watch it
            
            /* delete wd from wd_map , the file in the directory delete already */
            //int ans = knowledge_wd_map.erase(wd);
            log(FILE_NAME , INFO , "line %d ==> SUCCESS to delete directory(knowledge): %s",LINE_NUMBER , event->name);

        }
        else 
        {// delete a file
            std::string keytemp = event->name;
            size_t pos = keytemp.find_last_of('.');
            std::string key = keytemp.substr(0,pos);
            
            if(knowledge_maps.find(key) == knowledge_maps.end())
            {
                log(FILE_NAME , FATAL , "line %d ==> event(IN DELETE) happened but does not exist in knowledge_maps",LINE_NUMBER);
                return;
            }
            
            knowledge_maps.erase(key);
            
            log(FILE_NAME , INFO , "line %d ==> delete file(knowledge): %s",LINE_NUMBER,event->name);
        }
    }
    else if ( event->mask & IN_MODIFY ) 
    {
        if ( event->mask & IN_ISDIR ) 
        {// modify a dirs
            log(FILE_NAME , DEBUG , "line %d ==> modify directory(knowledge): %s",LINE_NUMBER,event->name);
            /*
            * add files in this dir and subdirs
            * need to handler the layer
            */
            add_knowledge_inotify_core(new_dir, fd , START_LAYER );
        }
        else
        {// modify a file
            //p->file_modify();
            log(FILE_NAME , DEBUG , "line %d ==> modify file(knowledge): %s",LINE_NUMBER,event->name);
            std::string keytemp = event->name;
            size_t pos = keytemp.find_last_of('.');
            std::string key = keytemp.substr(0,pos);
            
            knowledge_maps[key] = new_dir;
        }
    }
    else if ( event->mask & IN_DELETE_SELF ) 
    {
        if ( event->mask & IN_ISDIR ) 
        {// in delete a dir
            log(FILE_NAME , INFO , "line %d ==> delete directory self(knowledge): %s",LINE_NUMBER,event->name);
            //inotify_rm_watch( fd, wd );
            //p->dir_delete(new_dir, index);
        }
        else
        {// in delete a file
            std::string keytemp = event->name;
            size_t pos = keytemp.find_last_of('.');
            std::string key = keytemp.substr(0,pos);
            
            if(knowledge_maps.find(key) == knowledge_maps.end())
            {
                log(FILE_NAME , FATAL , "line %d ==>event(IN DELETE SELF) happened but does not exist in knowledge_maps",LINE_NUMBER);
                return;
            }
            
            knowledge_maps.erase(key);
            log(FILE_NAME , INFO , "line %d ==> delete file self(knowledge): %s",LINE_NUMBER,event->name);
        }
    }
    else if ( event->mask & IN_MOVED_FROM ) 
    {//
        if ( event->mask & IN_ISDIR ) 
        {// moved from a dir
            log(FILE_NAME , DEBUG , "line %d ==> in moved from directory(knowledge): %s",LINE_NUMBER,event->name);
            
            /* remove all the file from knowledge_maps */
            std::unordered_map<std::string , std::string>::iterator its = knowledge_maps.begin();
            for(; its != knowledge_maps.end() ; its++)
            {
                int len = new_dir.length();
                if(strncmp(new_dir.c_str() , (its->second).c_str() , len) == 0)
                {
                    knowledge_maps.erase(its);
                    its = knowledge_maps.begin();
                }
            }
        }
        else
        {// moved from a file
            std::string keytemp = event->name;
            size_t pos = keytemp.find_last_of('.');
            std::string key = keytemp.substr(0,pos);
            
            if(knowledge_maps.find(key) == knowledge_maps.end())
            {
                log(FILE_NAME , FATAL , "line %d ==> event(IN DELETE SELF) happened but does not exist in knowledge_maps",LINE_NUMBER);
                return;
            }
            
            knowledge_maps.erase(key);
            log(FILE_NAME , INFO , "line %d ==> file in moved from(knowledge) : %s",LINE_NUMBER,event->name);
        }
    }
    else if ( event->mask & IN_MOVED_TO ) 
    {
        if ( event->mask & IN_ISDIR ) 
        {// moved to a dir
            log(FILE_NAME , DEBUG , "line %d ==> in moved to a directory(knowledge): %s",LINE_NUMBER,event->name);
            /*
            * add files in this dir and subdirs
            * need to handler the layer
            */
            add_knowledge_inotify_core(new_dir, fd , START_LAYER );
        }
        else
        {// moved to a file
            std::string keytemp = event->name;
            size_t pos = keytemp.find_last_of('.');
            std::string key = keytemp.substr(0,pos);
            
            knowledge_maps[key] = new_dir;
            log(FILE_NAME , INFO , "line %d ==> in moved to a file(knowledge): %s",LINE_NUMBER,event->name);
        }
    }
    
    log(FILE_NAME , INFO , "line %d ==> knowledge_wd_map: ",LINE_NUMBER);
    std::unordered_map<int , std::string>::iterator its1 = knowledge_wd_map.begin();
    std::unordered_map<int , std::string>::iterator ite1 = knowledge_wd_map.end();
    for(; its1!= ite1 ; its1++)
    {
        log(FILE_NAME , INFO , "line %d ==> knowledge_wd ==> %d , knowledge_dir ==> %s",LINE_NUMBER,its1->first,(its1->second).c_str());
    }
    
    log(FILE_NAME , INFO , "line %d ==> knowledge_maps: ",LINE_NUMBER);
    std::unordered_map<std::string, std::string>::iterator its2 = knowledge_maps.begin();
    std::unordered_map<std::string, std::string>::iterator ite2 = knowledge_maps.end();
    for(; its2!= ite2 ; its2 ++)
    {
        log(FILE_NAME , INFO , "line %d ==> knowledge_key: %s ,knowledge_value: %s ",LINE_NUMBER,(its2->first).c_str(),(its2->second).c_str());
    }
}

