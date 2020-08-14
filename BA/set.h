//
// Created by hecong on 2020/8/12.
//

#ifndef ENPAC_2021_SET_H
#define ENPAC_2021_SET_H
//
// Created by hecong on 2020/7/30.
//
#include <set>
#include <iostream>
#include <vector>
using namespace std;
#define QUEUESIZE 8192   //must be 2^n
#define OK 1
#define ERROR 0

/*check if s2 is subseteq of s1, s2<=s1
 * */
template <typename T>
bool SUBSETEQ(const set<T> &s1, const set<T> &s2) {
    if(s1.size()<s2.size())
        return false;
    typename set<T>::const_iterator item1;
    typename set<T>::const_iterator item2;
    for(item2=s2.begin();item2!=s2.end();++item2) {        //for every item in s2, it must exist in s1
        for(item1=s1.begin();item1!=s1.end();++item1) {    //traverse over s1 to find if 'item2' exists in s1
            if(*item1 == *item2)                           //it exists and continues to check next 'item2';
                break;
        }
        if(item1==s1.end())                                //it doesn't exist, then return false;
            return false;
    }
    return true;
}

/*check if a belongs to set s
 * */
template <typename T>
bool BELONGTO(T a, const set<T> &s) {
    typename set<T>::const_iterator item;
    for(item=s.begin();item!=s.end();++item) {
        if(a == (*item)) {
            return true;
        }
    }
    return false;
}

template <typename T>
bool operator == (const set<T> &s1, const set<T> &s2) {
    if(s1.size()!=s2.size())
        return false;
    if(SUBSETEQ(s1,s2))
        return true;
    else
        return false;
}

template <typename T>
set<T> operator + (const set<T> &s1, const set<T> &s2) {
    set<T> result = s1;
    typename set<T>::const_iterator item2;
    for(item2=s2.begin();item2!=s2.end();++item2) {
        result.insert(*item2);
    }
    return result;
}

template <typename T>
ostream & operator << (ostream &out, const set<T> &s) {
    out<<"{";
    typename set<T>::const_iterator item;
    for(item=s.begin();item!=s.end();++item) {
        out<<*item<<",";
    }
    out<<"}";
    return out;
}

template <class T>
class CStack
{
private:
    vector<T> myDate;
public:
    T top() const;
    void push(const T &item);
    T pop();
    int size() const;
    bool empty() const;
    bool isinstack(const T& item) const;
    void clear();
};
template <class T>
T CStack<T>::top() const {
    return myDate.back();
}

template <class T>
void CStack<T>::push(const T &item) {
    myDate.push_back(item);
}

template <class T>
T CStack<T>::pop() {
    T item = myDate.back();
    myDate.pop_back();
    return item;
}

template <class T>
int CStack<T>::size() const {
    return myDate.size();
}

template <class T>
bool CStack<T>::empty() const {
    return myDate.empty();
}

template <class T>
void CStack<T>::clear() {
    myDate.clear();
}

template<class T>
bool CStack<T>::isinstack(const T &item) const {
    for(int i=0;i<myDate.size();++i)
    {
        if(myDate[i] == item)
            return true;
    }
    return false;
}

template <class T>
class CQueue
{
private:
    T myData[QUEUESIZE];
    int front;
    int rear;
public:
    CQueue();
    int DeQueue(T &item);
    int EnQueue(const T &item);
    bool empty();
    bool full();
    void clear();
};
template <class T>
CQueue<T>::CQueue() {
    front = rear = 0;
}
template <class T>
int CQueue<T>::DeQueue(T &item) {
    if(empty()) {
        return ERROR;
    }
    item = myData[front];
    front = (front+1) & (QUEUESIZE-1);
    return OK;
}
template <class T>
int CQueue<T>::EnQueue(const T &item) {
    if(full())
        return ERROR;
    myData[rear]=item;
    rear = (rear+1) & (QUEUESIZE-1);
    return OK;
}
template <class T>
bool CQueue<T>::empty() {
    if(front == rear)
        return true;
    else
        return false;
}
template <class T>
bool CQueue<T>::full() {
    if((rear+1)&(QUEUESIZE-1) == front)
        return true;
    else
        return false;
}
template <class T>
void CQueue<T>::clear() {
    front = rear = 0;
}
#endif //ENPAC_2021_SET_H
