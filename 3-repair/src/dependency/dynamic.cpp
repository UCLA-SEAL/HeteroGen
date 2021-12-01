//
// Created by qzhang on 3/10/21.
//

#include <iostream>
#include "../include/macro.h"

using namespace std;

struct ListNode{
  string type;
  char* arguments;
  ListNode *next;
};

void dynamic_dependency(void){
    ListNode *head = nullptr;
    //creat first node
    head = new ListNode;
    head->type = MAKE_STATIC;
    head->arguments = null;
    head->next = nullptr;

    ListNode *secondPtr = new ListNode;
    secondPtr->type = UPDATE;
    secondPtr->arguments = null;
    secondPtr->next = nullptr;
    head->next = secondPtr;

    ListNode *thirdPtr = new ListNode;
    thirdPtr->type = RESIZE;
    thirdPtr->arguments = null;
    thirdPtr->next = nullptr;
    secondPtr->next = thirdPtr;

    ListNode *forthPtr = new ListNode;
    forthPtr->type = INSERT;
    forthPtr->arguments = "exceptions";
    forthPtr->next = nullp
    thirdPtr->next = forthPtr;

    ListNode *fifthPtr = new ListNode;
    fifthPtr->type = INSERT;
    fifthPtr->arguments = "pragmas";
    fifthPtr->next = nullptr;
    forthPtr->next = fifthPtr;
}
