#pragma once
#include "iostream"
using namespace std;
template <class T>
class node                          //�ڵ�
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
	int Length();                       //��ȡ������
	void Create();                      //��������
	T Getvalue(int i);                  //�õ���i��Ԫ�ص�ֵ
	void Clear();                       //�������
	bool Empty() const;                 //�ж������Ƿ�Ϊ��
	void InsertLast(const T e);         //��β������һ��Ԫ��
	void InsertFirst(const T e);        //��ͷ������һ��Ԫ��
	void DeleteFirst();                 //��ͷɾ��һ��Ԫ��
	void DeleteLast();                  //��βɾ��Ԫ��
	void Display() const;               //��ʾ����
	node<T>* GetNode(int i);            //���ص�i���ڵ�
	void Reverse();                     //��������
	bool Find(const T e);               //�������в���ĳ��ֵ
	~list();                            //��������
private:
	node<T> *head;                        //ͷ�ڵ�    ͷ�ڵ���û�����ݵģ�ֻ�ǿ�ʼ��һ����ַ����
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
	cout << "������ֵ(��'ctrl+z'ֹͣ): ";
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
void list<T>::Display() const             //��ʾ����
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
node<T>* list<T>::GetNode(int i)              //���ص�i���ڵ�
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
void list<T>::Reverse()                   //��������
{
	node<T> *p, *q;
	p = head->next;
	int count = 0;
	while (p)                 //������ĳ���
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
bool list<T>::Find(const T e)             //�������в���ĳ��ֵ
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
T list<T>::Getvalue(int i)   //���㿪ʼ
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