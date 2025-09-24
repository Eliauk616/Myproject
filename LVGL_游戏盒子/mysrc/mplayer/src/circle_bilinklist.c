#include "head.h"

/*
	create_bilinklist:创建带头结点的双向链表(空链表)
	返回值：
		Bilinklist * 返回创建的头结点的地址
*/
Bilinklist *create_bilinklist()
{
    Bilinklist *head = (Bilinklist*)malloc(sizeof(Bilinklist));
    head->first = NULL;
    head->last = NULL;
    head->count = 0;
    
    return head;
}

/*
	create_node:创建数据结点
	@data: 待创建结点的数据,路径字符串
	返回值：
		Binode * 新创建的数据结点的地址
*/
Binode *create_node(Elemtype data)
{
    Binode *pnew = (Binode*)malloc(sizeof(Binode));
    pnew->data = (Elemtype)malloc(strlen(data) + 1);
    strcpy(pnew->data, data);
    pnew->next = NULL;
    pnew->prev = NULL;
    
    return pnew;
}

/*
	tail_insert: 尾插法往双向循环链表中插入新的数据结点
	@head: 链表的头结点的地址
	@pnew: 待插入的新的数据结点的首地址
	返回值：
		void
*/
void tail_insert(Bilinklist *head, Binode *pnew)
{
	if(head->first == NULL &&  head->last == NULL)//1.如果pnew是插入链表的第一个数据结点
    {
        head->first = head->last = pnew;
    }
    else//2.pnew是插入链表的第二个及往后的数据结点的情况
    {
        head->last->next = pnew;
        pnew->prev = head->last;
        head->last = pnew;
    }
    
    head->first->prev = head->last;
    head->last->next = head->first;
    head->count++;
}

/*
	head_insert: 头插法往双向循环链表中插入新的数据结点
	@head: 链表的头结点的地址
	@pnew: 待插入的新的数据结点的首地址
	返回值：
		void
*/
void head_insert(Bilinklist *head, Binode *pnew)
{
	if(head->first == NULL && head->last == NULL)//pnew第一个数据结点
    {
        head->first = head->last = pnew;
    }
    else//pnew第二个及往后数据结点的情况
    {
        pnew->next = head->first;
        head->first->prev = pnew;
        head->first = pnew;
    }
    
    head->first->prev = head->last;
    head->last->next = head->first;
    head->count++;
}

/* 
    print_circle_bilinklist: 打印带头结点的双向循环链表
    @head: 打印的链表的头结点的地址
    返回值：
        void
*/
void print_circle_bilinklist(Bilinklist *head)
{
    if(head == NULL || head->first == NULL)
    {
        return ;
    }

    Binode *p = head->first;
    while(p)
    {
        printf("%s\n", p->data);
        p = p->next;

        if(p == head->first) //完成遍历了一次又回到了第一个数据结点
        {
            break;
        }
    }
}

/*
	clear_bilinklist:清空带头结点的双向循环链表(删除所有的数据结点)
	@head: 待清空链表的头结点的地址
	返回值：
		void
*/
void clear_bilinklist(Bilinklist *head)
{
	if(head == NULL || head->first == NULL)
    {
        return ;
    }
    
    //先断开环
    head->first->prev = NULL;
    head->last->next = NULL;
    
    //按照双向链表清空方法清空
    Binode *pdel = head->first;
    while(pdel != NULL)
    {
        head->first = head->first->next;
        if(head->first != NULL)
        {
            head->first->prev = NULL; //如果有多个数据结点的情况
        }
        pdel->next = NULL;
        free(pdel->data);
        free(pdel);
  		pdel = head->first;
    }
    
    head->last = NULL;
    head->count = 0;
}

/*
	delete_bilinklist:删除整个链表
	@head: 头结点的地址
	返回值：
		Bilinklist *
*/
Bilinklist *delete_bilinklist(Bilinklist *head)
{
	if(head == NULL)
	{
		return head;
	}
	
	clear_bilinklist(head);
	
	free(head);
	head = NULL;
	return head;
}