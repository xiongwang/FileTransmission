/*
* Linpop follows the IP Messenger communication protocol (Draft-9)
* you can obtain a copy of Linpop at
*
*	http://code.google.com/p/linpop
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the Apache 2.0 License.
*/

#include "userAction.h"

user userList;

int insertUser(user *uList, user *target)
{
    user *pre, *cur;
    int compName, compHost, compAddr;

    if ((uList==NULL)||(target==NULL))
        return -1;

    if (target->peer.sin_addr.s_addr == htonl(INADDR_ANY))
        return -1;

    pre = uList;
    cur = uList->next;

    while (cur!=NULL)
    {
        compName=strncmp(cur->name, target->name, sizeof(cur->name));
        compHost=strncmp(cur->host, target->host, sizeof(cur->host));
        compAddr=memcmp(&cur->peer, &target->peer, sizeof(cur->peer));

        if (compName>0)
            break;

        if ((compName==0) && (compHost==0) && (compAddr==0))
        {
            if (cur->exit == 1)
                cur->exit = 0;
            return -1;
        }

        pre = cur;
        cur = cur->next;
    }

    target->next = pre->next;
    pre->next = target;
    return 0;
}

void destroyUsers(user *uList)
{
    user *cur;

    if (uList==NULL)
        return;

    while ((cur=uList->next)!=NULL)
    {
        uList->next = cur->next;
        free(cur);
    }
}

int listUsers(user **pusers, user *uList, int size, int flag) //flag==1时，仅仅列出所有用户
{
    user *cur;
    int order=0, tmp;
    const char name[]="UserName";
    const char host[]="HostName";
    const char ip[]="IP Address";

    if ((pusers==NULL)&&(flag==0))
        return 0;

    if (flag==0)
        bzero(pusers, sizeof(user*)*size);
    printf("\n---------------------------------------------------------\n");
    printf("\t%-12s%-20s%-15s\n", name, host, ip);

    cur = uList->next;
    while ((cur!=NULL)&&(size>1)) //最后一个并不用，可以用于判断结束
    {
        if (cur->exit == 0)
        {
            printf("%-8d%-12s%-20s%-15s\n"
                   , ++order
                   , cur->name
                   , cur->host
                   , (inet_ntoa(cur->peer.sin_addr)));

            if (flag==0)
            {
                cur->inUse = 1;
                *pusers++ = cur;
                size--;
            }
        }
        cur = cur->next;
    }

    printf("---------------------------------------------------------\n");

    if (order==0)
        printf("\nNo other users found.\n");

    return order;
}

int unListUsers(user **pusers, int num)
{
    user *cur;
    int tmp=0;

    if (pusers == NULL)
        return -1;

    while (num-->0)
    {
        cur = *pusers++;
        if (cur!=NULL)
        {
            cur->inUse = 0;
            tmp++;
        }
    }
    return tmp;
}


int delUser(user *uList, command *peercom)
{
    user *cur;
    int compName, compHost, compAddr;

    if (uList==NULL)
        return -1;

    cur = uList->next;

    while (cur!=NULL)
    {
        compName=strncmp(cur->name, peercom->senderName, sizeof(cur->name));
        compHost=strncmp(cur->host, peercom->senderHost, sizeof(cur->host));
        compAddr=memcmp(&cur->peer, &peercom->peer, sizeof(cur->peer));

        if (compName>0)
            return -1;

        if ((compName==0) && (compHost==0) && (compAddr==0))
        {
            cur->exit = 1;
            break;
        }
        cur = cur->next;
    }
    return 0;
}
