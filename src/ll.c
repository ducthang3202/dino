
#include "../header/ll.h"	


void LL_Add(LinkedList* ll, void* data){
        
    if(ll == NULL || data == NULL){
        return;
    }
    Node* n = malloc(sizeof(Node));

    if(n == NULL){
        SDL_Log("couldnt allocate mem for queue!");
        return;
    }

    ll->size++;
    n->data = data;
    
    // first data 
    if(ll->curr == NULL){
        n->next = ll->NIL;
        n->prev = ll->NIL;

        ll->NIL->prev = n;
        ll->NIL->next = n;
        ll->curr = n;
        return;
    }
    n->next = ll->NIL;
    ll->NIL->prev = n;
    n->prev = ll->curr;
    ll->curr->next = n;

    ll->curr = n;
}

// automatically searches for elements and removes it
void LL_Remove(LinkedList* ll, void* data){
    if(ll == NULL || ll->NIL == NULL)
        return;
    
    Node* curr = ll->NIL;
    while((curr = curr->next) != ll->NIL){
        if(curr->data == data){
            // relink list
            curr->prev->next = curr->next;
            curr->next->prev = curr->prev;

            //safely free data if any
            if(curr->data != NULL)
                free(curr->data);

            // edge case: if removed node is current node of ll -> update
            if(curr == ll->curr)
                ll->curr = ll->curr->prev;

            free(curr);
            ll->size--;
            break;
        }
    }
}
bool LL_Clean(LinkedList* ll){
    Node* curr = ll->NIL;
    while((curr = curr->next) != ll->NIL){

        if(curr->data != NULL)
            free(curr->data);

        free(curr);
        ll->size--;
    }
    if(ll->size == 0)
        return true;
    else
        return false;
}

LinkedList* LL_Create(){
    Node* n = malloc(sizeof(Node));
    n->next = n;
    n->prev = n;
    n->data = NULL;

    LinkedList* ll = malloc(sizeof(LinkedList));
    ll->curr = NULL;
    ll->NIL = n;
    ll->size = 0;
    return ll;
}

void* LL_Get(LinkedList* ll){
    if(ll == NULL){
        SDL_Log("cant use LinkedList if LinkedList is NULL!");
        return NULL;
    }
    if(ll->curr == NULL){
        return NULL;
    }

    ll->size--;
    Node* tmp = ll->curr;
    ll->NIL->prev = ll->curr->prev;
    ll->curr->prev->next = ll->NIL;
    void* d = tmp->data;
    free(tmp);
    return d;
}