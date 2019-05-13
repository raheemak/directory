#include <iostream>

using namespace std;

struct node
{
    int data;
    node *next;
};

class linked_list
{
private:
    node *head,*tail;
public:

    linked_list (int n){
	  head= NULL;
	  tail=NULL;
	  for (int x = 0; x <n; x++)
	    	add_node (x);
    }

    void add_node(int n)
    {
        node *tmp = new node;
        tmp->data = n;
        tmp->next = NULL;

        if(head == NULL)
        {
            head = tmp;
            tail = tmp;
        }
        else
        {
	    tail->next = tmp;
            tail = tail->next;
        }
    }

    //get next block and remove head
    int getNextBlock(){
    	int next;
	if (head == NULL)
		next = -1;
	else {
		next=head->data;
		if (head == tail){
			head = NULL; 
			tail = NULL;
		}
		else 
			head = head->next;
	}
	return next;
     } 

 	bool hasFreeSpace(){
	   return head != NULL;
    	}


	void printlist(){
		node *tmp=head;
		if (head==NULL)
			return;
		while (tmp->next != NULL){
			cout << endl << tmp->data;
			tmp = tmp->next;
		}
	}
	};
