#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "circle_bilinklist.h"

/*
    recursion_save_path: 遍历指定目录下所有为.mp4  .avi结尾文件，路径保存在链表中
    @dir_path: 就是需要遍历的目录的路径
    @head: 链表的头结点的地址，表示保存到哪个链表中
    返回值：
        void
*/
void recursion_save_path(const char *dir_path, Bilinklist *head)
{
    if(dir_path == NULL || head == NULL)
    {
        return ;
    }

    //1.打开一个目录
	DIR *drip = opendir(dir_path);
    if(drip == NULL)
    {
        perror("open dir fail");
        return ;
    }
    
    char path[256] = {0};
    struct dirent *dt;
    //2.读取目录中的目录项
    while((dt = readdir(drip)) != NULL) //循环每次读取目录中的一个目录项
    {
        if(strcmp(dt->d_name, ".")==0 || strcmp(dt->d_name, "..") == 0)//如果读取到的目录项为. 或者 .. 读取下一个目录项
        {
            continue;
        }
        
        memset(path, 0, sizeof(path));
        snprintf(path, sizeof(path), "%s/%s", dir_path, dt->d_name); //合成一条新路径
        
        //stat获取读取到的目录项的属性
        struct stat st;
        stat(path, &st);

        if(S_ISDIR(st.st_mode))//如果满足是目录，递归遍历
        {
            recursion_save_path(path, head);
        }
        else if(S_ISREG(st.st_mode) && (strncmp(path + strlen(path) - 4, ".mp4", 4) == 0 || strncmp(path + strlen(path) - 4, ".avi", 4) == 0))//满足普通文件并且以.bmp结尾文件，路径创建数据节点，插入链表中
        {
            Binode *pnew = create_node(path);
            tail_insert(head, pnew);
        }
       
    }
    
    //3.close,关闭目录文件
    closedir(drip);
}
