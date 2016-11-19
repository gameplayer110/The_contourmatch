#pragma once
#include "iostream"
using namespace std;
template <class T>
class node                          //节点
{
public:
	T data;
	node *next;                     //??
};

template <class T>
class list
{
public:
	list();
	int Length();                       //获取链表长度
	void Create();                      //创建链表
	T Getvalue(int i);                  //得到第i个元素的值
	void Clear();                       //清空链表
	bool Empty() const;                 //判断链表是否为空
	void InsertLast(const T e);         //从尾部插入一个元素
	void InsertFirst(const T e);        //从头部插入一个元素
	void DeleteFirst();                 //从头删除一个元素
	void DeleteLast();                  //从尾删除元素
	void Display() const;               //显示链表
	node<T>* GetNode(int i);            //返回第i个节点
	void Reverse();                     //链表逆置
	bool Find(const T e);               //在链表中查找某个值
	~list();                            //销毁链表
private:
	node<T> *head;                        //头节点    头节点是没有数据的，只是开始的一个地址罢了
};

template <class T>
list<T>::list()
{
	head = new node<T>;
	head->next = NULL;
}

template <class T>
int list<T>::Length()
{
	int num = 0;
	node<T>* p;
	p = head;
	while (p->next != NULL)
	{
		p = p->next;
		num++;
	}
	return num;
}

template <class T>
void list<T>::Create()
{
	node<T> *p, *q;
	p = head;
	q = new node<T>;
	cout << "请输入值(按'ctrl+z'停止): ";
	while (cin >> q->data)
	{
		p->next = q;
		p = q;
		q = new node<T>;
	}
	p->next = NULL;
}

template <class T>
bool list<T>::Empty()   const
{
	return (head->next == NULL);
}

template <class T>
void list<T>::InsertFirst(const T e)
{
	node<T> *p = new node<T>;
	p->data = e;
	p->next = head->next;
	head->next = p;
}

template <class T>
void list<T>::InsertLast(const T e)
{
	node<T> *p, *q;
	p = head;
	q = new node<T>;
	q->data = e;
	while (p->next)
	{
		p = p->next;
	}
	p->next = q;
	q->next = NULL;
}

template <class T>
void list<T>::DeleteFirst()
{
	head->next = head->next->next;
}

template <class T>
void list<T>::DeleteLast()
{
	node<T> *p;
	p = head;
	while (p->next->next)
		p = p->next;
	p->next = NULL;
}

template <class T>
void list<T>::Display() const             //显示链表
{
	node<T> *p;
	p = head->next;
	while (p)
	{
		cout << p->data << " ";
		p = p->next;
	}
}

template <class T>
node<T>* list<T>::GetNode(int i)              //返回第i个节点
{
	int k = 0;
	node<T> *p;
	p = head;
	while (p && k < i)
	{
		p = p->next;
		++k;
	}
	return p;
}

template <class T>
void list<T>::Reverse()                   //链表逆置
{
	node<T> *p, *q;
	p = head->next;
	int count = 0;
	while (p)                 //求链表的长度
	{
		p = p->next;
		count++;
	}
	int i, j;
	i = 1;
	j = count;
	while (i <= count / 2 && j >= count / 2)
	{
		p = GetNode(i);
		q = GetNode(j);
		T temp = p->data;
		p->data = q->data;
		q->data = temp;
		++i;
		--j;
	}
}

template <class T>
bool list<T>::Find(const T e)             //在链表中查找某个值
{
	bool flag = false;
	node<T> *p;
	p = head->next;
	while (p)
	{
		if (p->data == e)
		{
			flag = true;
			break;
		}
		p = p->next;
	}
	return flag;
}

template <class T>
T list<T>::Getvalue(int i)   //从零开始
{
	node<T> *p;
	p = head;
	for (int j = 0; j < i; j++)
	{
		p = p->next;
		if (p == NULL)
		{
			return 0;
		}
	}
	cout << p->data << endl;
	return p->data;
}
template <class T>
void list<T>::Clear()
{
	node<T> *p;
	while (head->next)
	{
		p = head->next;
		delete head;
		head = p;
	}
	head->data = NULL;
}

template <class T>
list<T>::~list()
{
	node<T> *p;
	while (head)
	{
		p = head->next;
		delete head;
		head = p;
	}
}