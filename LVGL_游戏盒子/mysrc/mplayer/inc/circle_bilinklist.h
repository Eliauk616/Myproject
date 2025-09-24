#ifndef __CIRCLE_BILINKLIST_H__
#define __CIRCLE_BILINKLIST_H__

typedef char* Elemtype;

//数据结点
typedef struct binode
{
	Elemtype data;//数据域  char *data;
    struct binode *next;//指针域，指向上一个数据结点的指针
    struct binode *prev;//指针域，指向下一个数据结点的指针
}Binode;

//头结点
typedef struct bilinklist
{
    Binode *first;//指向第一个数据结点的指针
    Binode *last;//指向最后一个数据结点的指针
    int count;//数据结点的个数
    //.....
}Bilinklist;

Bilinklist *create_bilinklist();
Binode *create_node(Elemtype data);
void tail_insert(Bilinklist *head, Binode *pnew);
void head_insert(Bilinklist *head, Binode *pnew);
void print_circle_bilinklist(Bilinklist *head);
void ascending_order_insert(Bilinklist *head, Binode *pnew);
void delete_all_node_x(Bilinklist *head, Elemtype x);
void clear_bilinklist(Bilinklist *head);
Bilinklist *delete_bilinklist(Bilinklist *head);

#endif