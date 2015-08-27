#pragma once
#ifndef __LLIST_INCLUDE__
#define __LLIST_INCLUDE__

template <typename T>
struct list_node
{
	list_node(T *obj) : data(obj), next(0), prev(0) { }

	struct list_node<T> *next;
	struct list_node<T> *prev;
	T *data;
};

template <typename T>
class llist
{
private:
	struct list_node<T> *head;
	struct list_node<T> *current;
	int elements;

public:
	llist() : head(0), current(0), elements(0) { }
	
	~llist()
	{
		clear();
	}

	int size() { return elements; }

	void clear()
	{
		while(head)
			remove(head);
	}

	void insert(T *obj)
	{
		struct list_node<T> *node = new list_node<T>(obj);

		if(head)
			head->prev = node;
		node->next = head;
		head = node;

		elements++;
	}

	void insert_last(T *obj)
	{
		struct list_node<T> *node;
		
		if(!head)
			head = new list_node<T>(obj);
		else
		{
			for(node = head; node->next; node = node->next)
				;
			node->next			= new list_node<T>(obj);
			node->next->prev	= node;
		}
	}

	void remove(struct list_node<T> *node)
	{
		if(node->prev == 0)
		{
			if(node->next == 0)
			{
				if(head == node)
					head = 0;
			}
			else
			{
				head = node->next;
				head->prev = node->next = 0;
			}
		}
		else
		{
			if(node->next)
				node->next->prev = node->prev;
			node->prev->next = node->next;
			node->next = node->prev = 0;
		}
		delete node;
		elements--;
	}

	void remove(T *obj)
	{
		struct list_node<T> *node;

		for(node = head; node; node = node->next)
		{
			if(node->data == obj)
			{
				remove(node);
				break;
			}
		}
	}

	void remove(int id)
	{
		struct list_node<T> *node;

		for(node = head; node; node = node->next)
		{
			if(*(node->data) == id)
			{
				remove(node);
				break;
			}
		}
	}

	void remove() { if(current) remove(current); }

	T *begin()
	{
		current = head;
		return current ? current->data : 0;
	}

	T *next()
	{
		if(current && current->next)
		{
			current = current->next;
			return current->data;
		} 
		return 0;
	}

	T* operator[](int x)
	{
		T *obj;
		int i;
		for(obj = this->begin(), i = 0; obj; obj = this->next(), i++)
			if(i == x)
				break;
		return obj;
	}

	int find(T *find)
	{
		T *obj;
		for(obj = this->begin(); obj; obj = this->next())
			if(find == obj)
				return 1;
		return 0;
	}
};

#endif
