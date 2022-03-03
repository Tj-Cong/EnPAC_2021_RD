//
// Created by hecong on 19-5-6.
//

#ifndef ENPAC_2020_2_0_PRODUCT_H
#define ENPAC_2020_2_0_PRODUCT_H

#include <iostream>
#include <algorithm>
#include <set>
#include <vector>
#include <string>
#include <unistd.h>
#include <signal.h>
#include <fstream>
#include "RG.h"
#include "BA/buchi.h"
#include <pthread.h>
#include <thread>

#define hash_table_num 1048576     //2^20
#define PSTACKSIZE 67108864       //2^26
#define HASHSIZE 1048576           //2^20
#define UNREACHABLE 0xffffffff
#define BOUND_BASE  2097151
#define CUTOFF 33554432

using namespace std;
extern bool timeflag;    //超时标志
extern NUM_t FIELDCOUNT;
extern NUM_t MARKLEN;
extern NUM_t placecount;
extern bool NUPN;
extern bool SAFE;
extern bool ready2exit;
extern bool SLICEPLACE;
extern bool SLICETRANSITION;
extern short int total_mem;
extern pid_t mypid;
extern Petri *petri;
void  sig_handler(int num);
template <class rgnode>
class Product;
template <class rgnode, class rg_T>
class Product_Automata;
/****************************PSTACK*******************************/
template <class rgnode>
class Pstacknode
{
public:
    bool deadmark;
    int id;
    int BAname_id;
    int fireptr;
    index_t next;

    //non-recursion extra information
    ArcNode *pba;
    rgnode *RGname_ptr;

    Pstacknode();
    ~Pstacknode(){};
    int NEXTFIREABLE();
};
template <class rgnode>
Pstacknode<rgnode>::Pstacknode() {
    RGname_ptr = NULL;
    next = UNREACHABLE;
    pba = NULL;
    fireptr = 0;
    deadmark = true;
}

template<class rgnode>
int Pstacknode<rgnode>::NEXTFIREABLE() {
    if(fireptr==0) {
//        if(STUBBORN && RGname_ptr->stubbornFlags==NULL) {
//            RGname_ptr->computeStubbornSet();
//        }
        int beginpos = 0;
        while(SLICETRANSITION && !petri->transition[beginpos].significant)
            beginpos++;
//        if(STUBBORN) {
//            for(beginpos;beginpos<petri->transitioncount;beginpos++){
//                if(RGname_ptr->stubbornFlags->test1(beginpos))
//                    break;
//            }
//        }
        if(RGname_ptr->isFirable(petri->transition[beginpos])) {
            deadmark = false;
            fireptr = beginpos;
            return fireptr;
        }
        fireptr++;
    }

    for(fireptr;fireptr<petri->transitioncount;++fireptr) {
//        if(STUBBORN && RGname_ptr->stubbornFlags->test0(fireptr))
//            continue;
//        if(STUBBORN && RGname_ptr->fireableFlags->test1(fireptr)) {
//            deadmark = false;
//            return fireptr;
//        }
        if(SLICETRANSITION && !petri->transition[fireptr].significant) {
            continue;
        }
        if(RGname_ptr->isFirable(petri->transition[fireptr])) {
            deadmark = false;
            return fireptr;
        }
    }
    return -1;
}

/*====================================================*/
template <class rgnode>
class PStack
{
public:
    Pstacknode<rgnode> **mydata;
    index_t *hashlink;
    index_t toppoint;

    PStack();
    index_t hashfunction(Pstacknode<rgnode> *qnode);
    Pstacknode<rgnode>* top();
    Pstacknode<rgnode>* pop();
    Pstacknode<rgnode>* search(Pstacknode<rgnode> *qnode);
    int push(Pstacknode<rgnode> *item);
    NUM_t size();
    bool empty();
    void clear();
    ~PStack();
};
template <class rgnode>
PStack<rgnode>::PStack() {
    toppoint = 0;
    mydata = new Pstacknode<rgnode>* [PSTACKSIZE];
    memset(mydata,0,sizeof(Pstacknode<rgnode>*)*PSTACKSIZE);
//    for(int i=0;i<PSTACKSIZE;++i)
//    {
//        mydata[i] = NULL;
//    }
    hashlink = new index_t[HASHSIZE];
    memset(hashlink,UNREACHABLE, sizeof(index_t)*HASHSIZE);
}
template <class rgnode>
index_t PStack<rgnode>::hashfunction(Pstacknode<rgnode> *qnode) {
    index_t hashvalue = qnode->RGname_ptr->Hash();
    index_t size = HASHSIZE-1;
    hashvalue = hashvalue & size;
    index_t Prohashvalue = (hashvalue+qnode->BAname_id)&size;
    return Prohashvalue;
}
template <class rgnode>
Pstacknode<rgnode>* PStack<rgnode>::top() {
    return mydata[toppoint-1];
}
template <class rgnode>
Pstacknode<rgnode>* PStack<rgnode>::pop() {
    Pstacknode<rgnode>* popitem = mydata[--toppoint];
    index_t hashpos = hashfunction(popitem);
    hashlink[hashpos] = popitem->next;
    mydata[toppoint]=NULL;
    return popitem;
}
template <class rgnode>
int PStack<rgnode>::push(Pstacknode<rgnode> *item) {
    if(toppoint>=PSTACKSIZE) {
        return ERROR;
    }
    index_t hashpos = hashfunction(item);
    item->next = hashlink[hashpos];
    hashlink[hashpos] = toppoint;
    mydata[toppoint++] = item;
    return OK;
}
template <class rgnode>
Pstacknode<rgnode>* PStack<rgnode>::search(Pstacknode<rgnode> *qnode) {
    index_t hashpos = hashfunction(qnode);
    index_t pos = hashlink[hashpos];
    Pstacknode<rgnode>* p;
    while(pos!=UNREACHABLE && !ready2exit)
    {
        p = mydata[pos];
        if(p->RGname_ptr==qnode->RGname_ptr && p->BAname_id==qnode->BAname_id)
        {
            return p;
        }
        pos = p->next;
    }
    return NULL;
}
template <class rgnode>
bool PStack<rgnode>::empty() {
    if(toppoint==0)
        return true;
    else
        return false;
}
template <class rgnode>
NUM_t PStack<rgnode>::size() {
    return toppoint;
}
template <class rgnode>
void PStack<rgnode>::clear() {
    toppoint = 0;
    for(int i=0;i<toppoint;++i)
    {
        if(mydata[i]!=NULL) {
            delete mydata[i];
            mydata[i] = NULL;
        }
    }
    memset(mydata,NULL,sizeof(Pstacknode<rgnode>*)*PSTACKSIZE);
    memset(hashlink,UNREACHABLE,sizeof(index_t)*HASHSIZE);
}
template <class rgnode>
PStack<rgnode>::~PStack() {
    for(int i=0;i<toppoint;++i)
    {
        if(mydata[i]!=NULL)
        {
            delete mydata[i];
            mydata[i]=NULL;
        }
    }
    delete [] mydata;
    mydata=NULL;
    delete [] hashlink;
    hashlink=NULL;
    MallocExtension::instance()->ReleaseFreeMemory();
}

/********************Product***********************/
template <class rgnode>
class Product  //交自动机的一个状态
{
public:
//    index_t id;           //交状态的大小标号
    rgnode *RGname_ptr;   //可达图状态所在位置，可以根据该指针索引到可达图状态
    int BAname_id;        //buchi自动机在自动机邻接表中的位置
    Product *hashnext;
public:
    Product();
    Product(Pstacknode<rgnode> *pnode);
};
template <class rgnode>
Product<rgnode>::Product() {
    RGname_ptr = NULL;
    hashnext = NULL;
}

template <class rgnode>
Product<rgnode>::Product(Pstacknode<rgnode> *pnode)
{
//    id = pnode->id;
    RGname_ptr = pnode->RGname_ptr;
    BAname_id = pnode->BAname_id;
    hashnext = NULL;
}
/****************************HashTable**************************/

template <class rgnode>
class hashtable
{
public:
    Product<rgnode> **table;
    NUM_t nodecount;
    NUM_t hash_conflict_times;
public:
    hashtable();
    void insert(Pstacknode<rgnode> *n);
    index_t hashfunction(Product<rgnode> *q);
    index_t hashfunction(Pstacknode<rgnode> *q);
    Product<rgnode> *search(Product<rgnode> *n);
    Product<rgnode> *search(Pstacknode<rgnode> *n);
    void pop(Product<rgnode> *n);
    void resetHash();
    ~hashtable();
};
template <class rgnode>
hashtable<rgnode>::hashtable() {
    table = new Product<rgnode>* [hash_table_num];
    int i=0;
    for(i;i<hash_table_num;i++)
    {
        table[i] = NULL;
    }
    nodecount = 0;
    hash_conflict_times = 0;
}

template <class rgnode>
hashtable<rgnode>::~hashtable() {
    int i=0;
    for(i;i<hash_table_num;i++)
    {
        if(table[i]!=NULL){
            Product<rgnode> *p = table[i];
            Product<rgnode> *q;
            while(p!=NULL){
                q = p->hashnext;
                delete p;
                p=NULL;
                p = q;
            }
        }
    }
    delete [] table;
    table=NULL;
    MallocExtension::instance()->ReleaseFreeMemory();
}

//template <class rgnode>
//void hashtable<rgnode>::insert(Product<rgnode> *q)
//{
//    int idex = hashfunction(q);
//    if(table[idex]!=NULL)
//        hash_conflict_times++;
//    Product<rgnode> *qs = new Product<rgnode>(q);
//    qs->hashnext = table[idex];
//    table[idex] = qs;
//    nodecount++;
//}

template <class rgnode>
void hashtable<rgnode>::insert(Pstacknode<rgnode> *q)
{
    int idex = hashfunction(q);
    if(table[idex]!=NULL)
        hash_conflict_times++;
    Product<rgnode> *qs = new Product<rgnode>(q);
    qs->hashnext = table[idex];
    table[idex] = qs;
    nodecount++;
}

template <class rgnode>
index_t hashtable<rgnode>::hashfunction(Product<rgnode> *q)
{
    index_t RGhashvalue;
    index_t size = hash_table_num-1;
    RGhashvalue = q->RGname_ptr->Hash();
    RGhashvalue = RGhashvalue & size;

    index_t Prohashvalue = RGhashvalue + q->BAname_id;
    Prohashvalue = Prohashvalue & size;
    return Prohashvalue;
}

template <class rgnode>
index_t hashtable<rgnode>::hashfunction(Pstacknode<rgnode> *q)
{
    index_t RGhashvalue;
    index_t size = hash_table_num-1;
    RGhashvalue = q->RGname_ptr->Hash();
    RGhashvalue = RGhashvalue & size;

    index_t Prohashvalue = RGhashvalue + q->BAname_id;
    Prohashvalue = Prohashvalue & size;
    return Prohashvalue;
}

template <class rgnode>
Product<rgnode> *hashtable<rgnode>::search(Product<rgnode> *n) {
    int idex = hashfunction(n);
    Product<rgnode> *p = table[idex];
    while (p != NULL) {
        if (p->BAname_id == n->BAname_id && p->RGname_ptr == n->RGname_ptr)
            return p;
        p = p->hashnext;
    }
    return NULL;
}

template <class rgnode>
Product<rgnode> *hashtable<rgnode>::search(Pstacknode<rgnode> *n) {
    int idex = hashfunction(n);
    Product<rgnode> *p = table[idex];
    while (p != NULL) {
        if (p->BAname_id == n->BAname_id && p->RGname_ptr == n->RGname_ptr)
            return p;
        p = p->hashnext;
    }
    return NULL;
}

template  <class rgnode>
void hashtable<rgnode>::resetHash()
{
    int i=0;
    for(i;i<hash_table_num;i++)
    {
        if(table[i]!=NULL){
            Product<rgnode> *p = table[i];
            Product<rgnode> *q;
            while(p!=NULL){
                q = p->hashnext;
                delete p;
                p=NULL;
                p = q;
            }
        }
    }
    memset(table,NULL,sizeof(Product<rgnode>*)*hash_table_num);
//    delete [] table;
//    MallocExtension::instance()->ReleaseFreeMemory();
//    table = new Product<rgnode>* [hash_table_num];
//    i=0;
//    for(i;i<hash_table_num;i++)
//    {
//        table[i] = NULL;
//    }
    nodecount = 0;
    hash_conflict_times = 0;
}
template <class rgnode>
void hashtable<rgnode>::pop(Product<rgnode> *n) {
    int idex = hashfunction(n);
    Product<rgnode> *p = table[idex];
    Product<rgnode> *q;
    //判断头结点是不是
    if(p->RGname_ptr == n->RGname_ptr && p->BAname_id == n->BAname_id)
    {
        q=p->hashnext;
        table[idex] = q;
        delete p;
        p=NULL;
        return;
    }
    //头结点不是
    q = p;
    p = p->hashnext;
    while(p!=NULL)
    {
        if(p->RGname_ptr == n->RGname_ptr && p->BAname_id == n->BAname_id){
            q->hashnext = p->hashnext;
            delete p;
            p = NULL;
            return;
        }
        q = p;
        p=p->hashnext;
    }
    cout<<"Couldn't delete from hashtable!"<<endl;
}

/************************Product_automata************************/
template <class rgnode, class rg_T>
class Product_Automata
{
private:
    vector<Product<rgnode>> initial_status;
    hashtable<rgnode> h;
    CStack<index_t> astack;
    CStack<index_t> dstack;
    PStack<rgnode> cstack;
    Petri *ptnet;
    rg_T *rg;
    StateBuchi *ba;
    int generatecount;
    bool result;
    int ret;
    vector<int> negpath;
    ofstream outcurdepth;

    //限界
    int bound;
    bool reachbound;
    //内存检测
    bool memory_flag;
    bool stack_flag;
    bool data_flag;
    bool consistency_flag;
    thread detect_mem_thread;
public:
    Product_Automata(Petri *pt, rg_T* r, StateBuchi *sba);
    void getProduct();         //合成交自动机
    Pstacknode<rgnode>* getNextChild(Pstacknode<rgnode>* q);
    void TCHECK(Pstacknode<rgnode>* p0);
    void TCHECK_BOUND(Pstacknode<rgnode>* p0);
    void UPDATE(Pstacknode<rgnode>* p0);
    int PUSH(Pstacknode<rgnode>* p0);
    void POP();
    void getProduct_Bound();                    //限界策略
    void addinitial_status(rgnode *initnode);  //生成交自动机的初始状态
    unsigned short ModelChecker(string propertyid,unsigned short each_run_time);  //最外层的函数
    bool isLabel(rgnode *state, int sj);  //判断能否合成交状态
    bool isLabel2(rgnode *state, int sj); //测试
    bool isLabel1(rgnode *state, int sj); //测试
    bool judgeF(string s);         //判断该公式是否为F类型的公式
    NUM_t sumtoken(string s, rgnode *state);   //计算s中所有库所的token和
    bool handleLTLF(string s, rgnode *state);
    bool handleLTLC(string s, rgnode *state);
    bool checkLTLF(rgnode *state, atomicmeta &am);
    bool checkLTLC(rgnode *state, atomicmeta &am);
    void handleLTLCstep(NUM_t &front_sum, NUM_t &latter_sum, string s, rgnode *state);
    int getresult();
    NUM_t getConflictTimes();
    int getgeneratecount();
    void printNegapth(ofstream &outpath);
    void detect_memory();
    double get_current_mem();
    ~Product_Automata();

};
template <class rgnode, class rg_T>
Product_Automata<rgnode,rg_T>::Product_Automata(Petri *pt, rg_T* r, StateBuchi *sba) {
    generatecount = 0;
    ptnet = pt;
    rg = r;
    ba = sba;
    result = true;
    memory_flag = true;
    stack_flag = true;
    reachbound = false;
    data_flag = true;
    consistency_flag = true;
    outcurdepth.open("curdepth.txt",ios::out);
}

/*bool Product_Automata::judgeF(string s)
 * function:判断一个公式是否属于F类型的公式
 * in: s,输入的公式；
 * out: true(F类型公式) or false(C类型公式)
 * */
template <class rgnode, class rg_T>
bool Product_Automata<rgnode,rg_T>::judgeF(string s) {
    int pos = s.find("<=");
    if (pos == string::npos)
    {
        return true;            //LTLFireability category
    }
    else return false;          //LTLCardinality category
}

/*void Product_Automata::ModelChecker(string propertyid, int &timeleft)
 * function: 最外层的函数，供main函数调用；
 * in: propertyid,公式字符串; timeleft,该公式剩余时间
 * out: void
 * */
template <class rgnode, class rg_T>
unsigned short Product_Automata<rgnode,rg_T>::ModelChecker(string propertyid, unsigned short each_run_time) {
    //预设超时机制
    signal(SIGALRM, sig_handler);
    alarm(each_run_time);
    timeflag = true;
    memory_flag = true;
    stack_flag = true;
    //核心部分
    result = true;
    getProduct();     //合成交自动机并进行搜索

    //打印结果
    string re;
    if(timeflag && memory_flag && stack_flag && data_flag && consistency_flag)
    {
        string nupn = NUPN?" USE_NUPN":"";
        string safe = SAFE?" STATE_COMPRESSION":"";
        string pinvar = PINVAR?" STATE_COMPRESSION":"";
        string longbitplace = LONGBITPLACE?" LONGBITPLACE":"";
//        string stubborn = STUBBORN?" STUBBORN":"";
        string sliceplace = SLICEPLACE ? " SLICEPLACE" : "";
        string slicetransition = SLICETRANSITION ? " SLICETRANSITION"+to_string(petri->sliceTransitionCount)+"/"+to_string(petri->transitioncount) : "";
        if(result)
        {
            re="TRUE";
            cout << "FORMULA " + propertyid + " " + re<<nupn+safe+pinvar+sliceplace+slicetransition;
            ret = 1;
        }
        else
        {
            re="FALSE";
            cout << "FORMULA " + propertyid + " " + re<<nupn+safe+pinvar+sliceplace+slicetransition;
            ret = 0;
        }
    }
    else if(!memory_flag)
    {
        cout<<"FORMULA "+propertyid+" "+"CANNOT_COMPUTE MEMORY";
        ret = -1;
    }
    else if(!stack_flag)
    {
        cout<<"FORMULA "+propertyid+" "+"CANNOT_COMPUTE STACK";
        ret = -1;
    }
    else if(!data_flag)
    {
        cout<<"FORMULA "+propertyid+" "+"CANNOT_COMPUTE DATA";
        ret = -1;
    }
    else if(!consistency_flag)
    {
        cout<<"FORMULA "+propertyid+" "+"CANNOT_COMPUTE CONSISTENCY";
        ret = -1;
    }
    else if(!timeflag)
    {
        cout<<"FORMULA "+propertyid+" "+"CANNOT_COMPUTE TIME";
        ret = -1;
    }
    unsigned short timeleft=alarm(0);
    return (each_run_time - timeleft);
}

/*void Product_Automata::getProduct()
 * function: 合成交自动机并进行搜索
 * */
template <class rgnode, class rg_T>
void Product_Automata<rgnode,rg_T>::getProduct() {
        detect_mem_thread = thread(&Product_Automata::detect_memory, this);
        //如果还未得到rg的初始状态，那么就生成他的初始状态
        if (rg->initnode == NULL) {
            rg->RGinitialnode();
        }

        //生成交自动机的初始状态
        addinitial_status(rg->initnode);

        //从初始状态开始搜索
        int i = 0;
        int end = initial_status.size();
        for (i; i < end; i++) {
            Pstacknode<rgnode> *init = new Pstacknode<rgnode>;
            init->id = 0;
            init->RGname_ptr = initial_status[i].RGname_ptr;
            init->BAname_id = initial_status[i].BAname_id;
            init->pba = ba->vertics[init->BAname_id].firstarc;
            TCHECK(init);
            if (ready2exit)  //如果已经出结果或超时，则退出
            {
                break;
            }
        }
        ready2exit = true;
        detect_mem_thread.join();
}

template <class rgnode, class rg_T>
Pstacknode<rgnode>* Product_Automata<rgnode,rg_T>::getNextChild(Pstacknode<rgnode> *q) {
    int firenum;
    while((firenum=q->NEXTFIREABLE())!=-1) {
        bool exist;
        rgnode *rgseed = rg->RGcreatenode(q->RGname_ptr,firenum,exist);
        if(rgseed == NULL)
        {
            data_flag = false;
            ready2exit = true;
            return NULL;
        }
        while(q->pba)
        {
            if(isLabel(rgseed,q->pba->destination))
            {
                Pstacknode<rgnode> *qs = new Pstacknode<rgnode>;
                qs->RGname_ptr = rgseed;
                qs->BAname_id = q->pba->destination;
                qs->pba = ba->vertics[qs->BAname_id].firstarc;

                /*print*/
//                       qs->RGname_ptr->printMarking(placecount);
//                       cout<<qs->BAname_id<<endl;

                if(qs->pba == NULL) {
                    cerr<<"detect non-sense sba state!"<<endl;
                    exit(6);
                }

                q->pba = q->pba->next;
                return qs;
            }
            q->pba = q->pba->next;
        }
        q->fireptr++;
        q->pba = ba->vertics[q->BAname_id].firstarc;
    }
    if(q->deadmark) {
        while(q->pba)
        {
            if(isLabel(q->RGname_ptr,q->pba->destination))
            {
                //可以生成交状态
                Pstacknode<rgnode> *qs = new Pstacknode<rgnode>;
                qs->RGname_ptr = q->RGname_ptr;
                qs->BAname_id = q->pba->destination;
                qs->pba = ba->vertics[qs->BAname_id].firstarc;

                /*print*/
//                   qs->RGname_ptr->printMarking(placecount);
//                   cout<<qs->BAname_id<<endl;

                if(qs->pba == NULL) {
                    cerr<<"detect non-sense sba state!"<<endl;
                    exit(6);
                }

                q->pba = q->pba->next;
                return qs;
            }
            q->pba = q->pba->next;
        }
        return NULL;
    }
    else {
        return NULL;
    }
}

template <class rgnode, class rg_T>
void Product_Automata<rgnode,rg_T>::TCHECK(Pstacknode<rgnode> *p0) {
        PUSH(p0);
        while(!dstack.empty()&&!ready2exit)
        {
            Pstacknode<rgnode> *q = cstack.mydata[dstack.top()];
            Pstacknode<rgnode> *qs = getNextChild(q);
            if(qs==NULL)
            {
                if(!data_flag)
                    return;
                POP();
                continue;
            }
            else{
                if(h.search(qs)!=NULL) {
                    delete qs;
                    qs=NULL;
                    continue;
                }
                Pstacknode<rgnode> *existpos = cstack.search(qs);
                if(existpos!=NULL) {
                    UPDATE(existpos);
                    delete qs;
                    qs = NULL;
                    continue;
                }
                PUSH(qs);
//                Pstacknode<rgnode> *existpos = cstack.search(qs);
//                if(existpos!=NULL)
//                {
//                    UPDATE(existpos);
//                    delete qs;
//                    qs = NULL;
//                    continue;
//                }
//                if(h.search(qs)==NULL)
//                {
//                    PUSH(qs);
//                    continue;
//                }
//                delete qs;
//                qs = NULL;
            }
        }
}

template <class rgnode, class rg_T>
void Product_Automata<rgnode,rg_T>::TCHECK_BOUND(Pstacknode<rgnode> *p0) {
    PUSH(p0);
    while(!dstack.empty()&&!ready2exit)
    {
        Pstacknode<rgnode> *q = cstack.mydata[dstack.top()];
        Pstacknode<rgnode> *qs = getNextChild(q);
        if(qs==NULL)
        {
            if(!data_flag)
                return;
            POP();
            continue;
        }
        else
        {
            if(h.search(qs)!=NULL) {
                /*this node exists in hash table, it means the scc
                 * which includes this node doesn't have a counterexample,
                 * so there is no need to search qs again*/
                delete qs;
                qs=NULL;
                continue;
            }
            Pstacknode<rgnode> *existpos = cstack.search(qs);
            if(existpos!=NULL) {
                UPDATE(existpos);
                delete qs;
                qs=NULL;
                continue;
            }
            if(dstack.size()>=bound) {
                reachbound = true;
                delete qs;
                continue;
            }
            PUSH(qs);
//            Pstacknode<rgnode> *existpos = cstack.search(qs);
//            if(existpos!=NULL)
//            {
//                UPDATE(existpos);
//                delete qs;
//                qs=NULL;
//                continue;
//            }
//            if(h.search(qs)==NULL)
//            {
//                if(dstack.size()>=bound) {
//                    reachbound = true;
//                    continue;
//                }
//
//                PUSH(qs);
//                continue;
//            }
//            delete qs;
//            qs=NULL;
        }
    }
}

template <class rgnode, class rg_T>
int Product_Automata<rgnode,rg_T>::PUSH(Pstacknode<rgnode> *p0) {
    p0->id = cstack.toppoint;
    dstack.push(cstack.toppoint);
    if(ba->vertics[p0->BAname_id].accepted)
        astack.push(cstack.toppoint);
    if(cstack.push(p0)==ERROR)
    {
        stack_flag = false;
        ready2exit = true;
        return ERROR;
    }
    return OK;
}

template <class rgnode, class rg_T>
void Product_Automata<rgnode,rg_T>::POP() {
    index_t p = dstack.pop();
    if(cstack.mydata[p]->id == p)
    {
        //强连通分量的根节点
        while(cstack.toppoint>p)
        {
            Pstacknode<rgnode>* popitem = cstack.pop();
            h.insert(popitem);
            delete popitem;
            popitem=NULL;
        }
    }
    if(!astack.empty() && astack.top()==p)
        astack.pop();
    if(!dstack.empty())
        UPDATE(cstack.mydata[p]);
}

template <class rgnode, class rg_T>
void Product_Automata<rgnode,rg_T>::UPDATE(Pstacknode<rgnode> *p0) {
    if(p0 == NULL)
        return;
    index_t dtop = dstack.top();
    if(p0->id<=cstack.mydata[dtop]->id)
    {
        if(!astack.empty() && p0->id <= astack.top())
        {
            result = false;
            ready2exit = true;
        }
        cstack.mydata[dtop]->id = p0->id;
    }
}
/*void Product_Automata::getProduct()
 * function: 合成交自动机并进行搜索
 * */
template <class rgnode, class rg_T>
void Product_Automata<rgnode,rg_T>::getProduct_Bound() {
    detect_mem_thread = thread(&Product_Automata::detect_memory,this);

    //如果还未得到rg的初始状态，那么就生成他的初始状态
    if(rg->initnode == NULL){
        rg->RGinitialnode();
    }

    //生成交自动机的初始状态
    addinitial_status(rg->initnode);

    //从初始状态开始搜索

    bound = BOUND_BASE;
    //限界
    while(bound <= CUTOFF)
    {
        reachbound = false;
        int i = 0;
        int end = initial_status.size();
        for(i;i<end;i++)
        {
            Pstacknode<rgnode> *init = new Pstacknode<rgnode>;
            init->id= 0;
            init->RGname_ptr = initial_status[i].RGname_ptr;
            init->BAname_id = initial_status[i].BAname_id;
            init->pba = ba->vertics[init->BAname_id].firstarc;
            init->fireptr = init->RGname_ptr->fireset.begin();
            TCHECK_BOUND(init);
            if(ready2exit || !data_flag)  //如果已经出结果或超时，则退出
                break;
        }
        if(ready2exit || !data_flag)  //如果已经出结果或超时，则退出
            break;
        if(!reachbound)
        {
            result = true;
            break;
        }
        bound = bound*2;
        h.resetHash();
        cstack.clear();
        astack.clear();
        dstack.clear();

    }

    ready2exit = true;

    if(bound>CUTOFF)
        cout<<"out of the bound"<<endl;
    detect_mem_thread.join();
}

/*void Product_Automata::addinitial_status(RGNode *initnode)
 * function: 生成交自动机的初始状态，并加入到initial_status数组中
 * in: initnode,可达图的初始节点
 * out: void
 * */
template <class rgnode, class rg_T>
void Product_Automata<rgnode,rg_T>::addinitial_status(rgnode *initnode) {
    //遍历buchi自动机的所有状态，找到所有初始状态
    for(int i=0; i<ba->vex_num; i++)
    {
        if(ba->vertics[i].id == -1)
            continue;
        if(ba->vertics[i].initial)
        {
            //如果是初始状态，就和可达图的初始状态生成交自动机的初始状态
            if(isLabel(initnode, i))
            {
                //生成交自动机初始状态
                Product<rgnode> N;
                N.BAname_id = i;
                N.RGname_ptr = initnode;
                //加入初始状态数组中
                initial_status.push_back(N);
            }
        }
    }
}

/*bool Product_Automata::isLabel(RGNode *state, int sj)
 * function: 判断可达图的一个状态和buchi自动机的一个状态能否合成交状态
 * in: state,可达图状态指针，指向要合成的可达图状态
 *     sj,buchi自动机状态在邻接表中的序号
 * out: true(可以合成交状态) or false(不能合成状态)
 * */
template <class rgnode, class rg_T>
bool Product_Automata<rgnode,rg_T>::isLabel2(rgnode *state, int sj) {

    //取出自动机状态中的label
    string str = ba->vertics[sj].label;
    if(str == "true")
        return true;

    bool mark = false;
    while (1)
    {
        int pos = str.find_first_of("&&");

        if (pos == string::npos)     //最后一个原子命题
        {
            //cout<<str;
            if (judgeF(str))
            {
                /*a && b && c:
                 * true: 都为true
                 * false： 只要有一个为false
                 * */
                mark = handleLTLF(str, state);
                if(mark == false)
                    return false;
            }
            else {  //C类型公式
                //cout<<"c����";
                mark = handleLTLC(str, state);
                if(mark == false)
                    return false;
            }
            break;
        }

        string subprop = str.substr(0, pos);    //取出一个原子命题
        //cout<<subprop<<" ";

        if (judgeF(subprop))   //当前公式为F类型公式
        {
            /*a && b && c:
            * true: 都为true
            * false： 只要有一个为false
            * */
            mark = handleLTLF(subprop, state);
            if (mark == false)
            {
                //当前原子命题不成立
                return false;
            }
        }
        else                   //当前公式为C类型公式
        {
            /*a && b && c:
            * true: 都为true
            * false： 只要有一个为false
            * */
            mark = handleLTLC(subprop, state);
            if (mark == false)
            {
                //有一个原子命题不成立
                return false;
            }
        }
        //cout<<endl;
        str = str.substr(pos + 2, str.length() - pos -2);
    }
    return true;
}

/*bool Product_Automata::handleLTLF(string s, RGNode *state)
 * function:判断F类型中的一个原子命题在状态state下是否成立
 * in: s是公式的一小部分，一个原子命题； state，状态
 * out: true(成立) or false(不成立)
 * */
template <class rgnode, class rg_T>
bool Product_Automata<rgnode,rg_T>::handleLTLF(string s, rgnode *state) {
    if(s[0] == '!') //前面带有'!'的is-fireable{}
    {
        /*!{t1 || t2 || t3}：
         * true：t1不可发生 并且 t2不可发生 并且 t3不可发生
         * false： 只要有一个能发生
         * */
        s = s.substr(2,s.length()-2); //去掉“!{”
        while(1)
        {
            int pos = s.find_first_of(",");
            if (pos < 0)
                break;
            string subs = s.substr(0, pos);            //取出一个变迁

            int idex = ptnet->getTPosition(subs);      //得到变迁在petri网中的index
            if(idex == INDEX_ERROR){
                consistency_flag = false;
                ready2exit = true;
                return false;
            }

            if(state->isFirable(ptnet->transition[idex])) {
                return false;
            }

            s = s.substr(pos + 1, s.length() - pos);
        }
        return true;

    } else{         //单纯的is-fireable{}原子命题
        /*{t1 || t2 || t3}:
	     * true: 只要有一个能发生
	     * false: 都不能发生
	     * */
        s = s.substr(1, s.length() - 1);//去掉‘{’

        while(1)
        {
            int pos = s.find_first_of(",");
            if (pos < 0)
                break;
            string subs = s.substr(0, pos);  //取出一个变迁

            int idex = ptnet->getTPosition(subs);  //变迁的序列号
            if(idex == INDEX_ERROR){
                consistency_flag = false;
                ready2exit = true;
                return false;
            }

            if(state->isFirable(ptnet->transition[idex])) {
                return true;
            }

            //走到这里表示取出来的变迁不可发生
            s = s.substr(pos + 1, s.length() - pos);
        }
        return false;
    }
}

/*bool Product_Automata::handleLTLC(string s, RGNode *state)
 * function: 判断C类型公式中的一个原子命题s在状态state下是否成立
 * in: s,原子命题； state,状态
 * out: true(成立) or false(不成立)
 * */
template <class rgnode,class rg_T>
bool Product_Automata<rgnode,rg_T>::handleLTLC(string s, rgnode *state) {
    NUM_t front_sum, latter_sum;     //前半部分和 后半部分和
    if(s[0] == '!')
    {
        /*!(front <= latter)
	     * true:front > latter
	     * false:front <= latter
	     * */
        s = s.substr(2, s.length() - 2);   //去除"!{"
        handleLTLCstep(front_sum, latter_sum, s, state);
        if(front_sum <= latter_sum)
            return false;
        else
            return true;
    }
    else {
        /*(front <= latter)
         * true:front <= latter
         * false:front > latter
         * */
        s = s.substr(1, s.length() - 1);   //去除"{"
        handleLTLCstep(front_sum, latter_sum, s, state);
        if(front_sum <= latter_sum)
            return true;
        else
            return false;
    }
}

/*void handleLTLCstep(int &front_sum, int &latter_sum, string s, RGNode *state)
 * function:计算在状态state下，C公式"<="前面库所的token和front_sum和后面库所的token和latter_sum
 * in: s,公式； state,状态
 * out: front_sum前半部分的和, latter_sum后半部分的和
 * */
template <class rgnode,class rg_T>
void Product_Automata<rgnode,rg_T>::handleLTLCstep(NUM_t &front_sum, NUM_t &latter_sum, string s, rgnode *state) {
    if(s[0] == 't')   //前半部分是token-count的形式
    {
        int pos = s.find_first_of("<=");           //定位到<=前
        string s_token = s.substr(12, pos - 13);   //去除"token-count(" ")"  ֻ只剩p1,p2,
        front_sum = sumtoken(s_token, state);      //计算token和

        //计算后半部分的和
        s = s.substr(pos + 2, s.length() - pos - 2); //取得后半部分，若是常数 则是常数+}

        if(s[0] == 't')   //后半部分是token-count
        {
            string s_token = s.substr(12, s.length() - 14);
            latter_sum = sumtoken(s_token, state);
        }
        else{             //后半部分是常数
            s = s.substr(0, s.length() - 1);        //去除后面的}
            latter_sum = atoi(s.c_str());
        }
    }
    else{             //前半部分是常数，那后半部分肯定是token-count
        //处理前半部分  现有的串 是从常数开始的
        int pos = s.find_first_of("<=");//定位到<=,取其前半部分
        string num = s.substr(0, pos);//取出常数
        front_sum = atoi(num.c_str());
        //	 cout<< " "<<front_sum<<" ";

        //处理后半部分
        s = s.substr(pos + 14, s.length() - pos - 16);
        //	 cout<<" "<<s<<" ";
        latter_sum = sumtoken(s, state);
    }
}

/*int Product_Automata::sumtoken(string s, RGNode *state)
 * function: 计算在状态state下，某些库所的token和
 * in: s:p1,p2,p3,p4,这样的库所名字字符串；state，状态
 * out: 库所的token和
 * */
template <class rgnode,class rg_T>
NUM_t Product_Automata<rgnode, rg_T>::sumtoken(string s, rgnode *state) {
    NUM_t sum = 0;
    if (ptnet->NUPN || ptnet->SAFE) {
        while (1) {
            int pos = s.find_first_of(",");
            if (pos == string::npos)
                break;
            string subs = s.substr(0, pos);        //取得一个p1
            index_t idex = ptnet->getPPosition(subs);  //得到该库所的索引号

            if (idex == INDEX_ERROR) {
                consistency_flag = false;
                ready2exit = true;
                return 0;
            }

            sum += state->readPlace(idex)?1:0;

            //将前面的用过的P1去除 从p2开始作为新的s串
            s = s.substr(pos + 1, s.length() - pos);
        }
    }
    else if(ptnet->PINVAR || LONGBITPLACE) {
        while (1) {
            int pos = s.find_first_of(",");
            if (pos == string::npos)
                break;
            string subs = s.substr(0, pos);        //取得一个p1
            index_t idex = ptnet->getPPosition(subs);  //得到该库所的索引号

            if (idex == INDEX_ERROR) {
                consistency_flag = false;
                ready2exit = true;
                return 0;
            }

            sum += state->readPlace(idex);

            //将前面的用过的P1去除 从p2开始作为新的s串
            s = s.substr(pos + 1, s.length() - pos);
        }
    }
    else {
        while (1) {
            int pos = s.find_first_of(",");
            if (pos == string::npos)
                break;
            string subs = s.substr(0, pos);        //取得一个p1
            index_t idex = ptnet->getPPosition(subs);  //得到该库所的索引号

            if (idex == INDEX_ERROR) {
                consistency_flag = false;
                ready2exit = true;
                return 0;
            }

            Mark ptokennum;
            memcpy(&ptokennum,&state->marking[idex],sizeof(Mark));
            sum += ptokennum;

            //将前面的用过的P1去除 从p2开始作为新的s串
            s = s.substr(pos + 1, s.length() - pos);
        }
    }

    return sum;
}

template <class rgnode,class rg_T>
int Product_Automata<rgnode,rg_T>::getresult() {
    return ret;
}

template <class rgnode,class rg_T>
NUM_t Product_Automata<rgnode,rg_T>::getConflictTimes() {
    return h.hash_conflict_times;
}


template <class rgnode,class rg_T>
int Product_Automata<rgnode,rg_T>::getgeneratecount() {
    return generatecount;
}

template <class rgnode,class rg_T>
void Product_Automata<rgnode,rg_T>::printNegapth(ofstream &outpath) {
    vector<int>::reverse_iterator iter;
    for(iter=negpath.rbegin(); iter!=negpath.rend(); iter++){
        outpath<<(*iter)<<endl;
    }
}

template <class rgnode,class rg_T>
void Product_Automata<rgnode,rg_T>::detect_memory()
{
    for(;;)
    {
        int size=0;
        char filename[64];
        sprintf(filename,"/proc/%d/status",mypid);
        FILE *pf = fopen(filename,"r");
        if(pf == nullptr)
        {
            //cout<<"未能检测到enPAC进程所占内存"<<endl;
            pclose(pf);
        } else{
            char line[128];
            while(fgets(line,128,pf) != nullptr)
            {
                if(strncmp(line,"VmRSS:",6) == 0)
                {
                    int len = strlen(line);
                    const char *p=line;
                    for(;std::isdigit(*p) == false;++p) {}
                    line[len-3]=0;
                    size = atoi(p);
                    break;
                }
            }
            fclose(pf);
            size = size/1024;
            if(100*size/total_mem > 85)
            {
                memory_flag = false;
                ready2exit = true;
                break;
            }

        }
        if(ready2exit)
        {
            break;
        }
        usleep(500);
    }
}

template <class rgnode,class rg_T>
Product_Automata<rgnode,rg_T>::~Product_Automata() {

}



/*bool Product_Automata::isLabel2(RGNode *state, int sj)
 * function: 判断可达图的一个状态和buchi自动机的一个状态能否合成交状态
 * in: state,可达图状态指针，指向要合成的可达图状态
 *     sj,buchi自动机状态在邻接表中的序号
 * out: true(可以合成交状态) or false(不能合成状态)
 * */
template <class rgnode, class rg_T>
bool Product_Automata<rgnode, rg_T>::isLabel1(rgnode *state, int sj) {
    if (ba->vertics[sj].invalid)
        return false;
    if (ba->vertics[sj].label=="true")
        return true;

    atomictable &AT = *(ba->pAT);
    const vector<Atomic> &links = ba->vertics[sj].links;
    Atomic temp;
    int atomic_num = links.size();
    bool atomicTrue= false;
    unsigned int left, right;
    Mark ptokennum;
    int i, j;
    for (i = 0; i < atomic_num; i++) {
        temp = links[i];
        if (AT.atomics[temp.atomicmeta_link].mytype == PT_FIREABILITY) {
            //LTL---F
            atomicTrue = AT.atomics[temp.atomicmeta_link].last_check = false;

            int t_num = AT.atomics[temp.atomicmeta_link].fires.size();
            const vector<unsigned int> &fires = AT.atomics[temp.atomicmeta_link].fires;
            for (j = 0; j < t_num; j++)
                if (state->isFirable(ptnet->transition[fires[j]])) {
                    atomicTrue = true;
                    break;
                }
        }
        else if (AT.atomics[temp.atomicmeta_link].mytype == PT_CARDINALITY) {
            //LTL---C
            const cardexp &l_exp = AT.atomics[temp.atomicmeta_link].leftexp;
            const cardexp &r_exp = AT.atomics[temp.atomicmeta_link].rightexp;
            left = right = 0;

            //left
            if (l_exp.constnum != -1) {
                left = l_exp.constnum;
            } else {
                const cardmeta *p = l_exp.expression;
                while (p) {
                    //memcpy(&ptokennum, &state->marking[p->placeid], sizeof(Mark));
                    left += state->readPlace(p->placeid);
                    p = p->next;
                }
            }

            //right
            if (r_exp.constnum != -1) {
                right = r_exp.constnum;
            } else {
                const cardmeta *p = r_exp.expression;
                while (p) {
                    right += state->readPlace(p->placeid);
                    p = p->next;
                }
            }

            atomicTrue = left <= right;
        }

        if ((atomicTrue&&temp.negation)||(!atomicTrue&&!temp.negation))
            return false;
    }

    return true;
}

/*bool Product_Automata::isLabel(RGNode *state, int sj)
 * function: 判断可达图的一个状态和buchi自动机的一个状态能否合成交状态
 * in: state,可达图状态指针，指向要合成的可达图状态
 *     sj,buchi自动机状态在邻接表中的序号
 * out: true(可以合成交状态) or false(不能合成状态)
 * */
template<class rgnode, class rg_T>
bool Product_Automata<rgnode, rg_T>::isLabel(rgnode *state, int sj) {
    if (ba->vertics[sj].invalid)
        return false;
    if (ba->vertics[sj].label == "true")
        return true;

    // debug
    //static ofstream debugoutput("checkpoint.txt");
//    extern ofstream debugout;

    // def
    atomictable &AT = *(ba->pAT);
    const vector<Atomic> &links = ba->vertics[sj].links;
    Atomic temp;
    int atomic_num = links.size();
    bool atomicTrue = false;
    int i, j;
    for (i = 0; i < atomic_num; i++){
        temp = links[i];

        if (AT.atomics[temp.atomicmeta_link].last_check_avaliable)
            atomicTrue = AT.atomics[temp.atomicmeta_link].last_check;
        else if (AT.atomics[temp.atomicmeta_link].mytype == PT_FIREABILITY)
            //call F handle
            atomicTrue = checkLTLF(state, AT.atomics[temp.atomicmeta_link]);
        else if (AT.atomics[temp.atomicmeta_link].mytype == PT_CARDINALITY)
            //call C handle
            atomicTrue = checkLTLC(state, AT.atomics[temp.atomicmeta_link]);

        if ((atomicTrue&&temp.negation)||(!atomicTrue&&!temp.negation)) {
//            debugout << "F " << ba->vertics[sj].label << endl;
            return false;
        }
    }
//    debugout << "T " << ba->vertics[sj].label << endl;
    return true;
}
// used above only (new HandleLTLF) for atomics table
template<class rgnode, class rg_T>
bool Product_Automata<rgnode, rg_T>::checkLTLF(rgnode *state, atomicmeta &am) {
    bool ans = false;
    int t_num = am.fires.size();
    int i;
    for (i = 0; i < t_num; i++) {
        if (state->isFirable(ptnet->transition[am.fires[i]])) {
            ans = true;
            break;
        }
    }

    am.last_check_avaliable = false;
    am.last_check = ans;
    return ans;
}
// used above only (new HandleLTLC) for atomics table
template<class rgnode, class rg_T>
bool Product_Automata<rgnode, rg_T>::checkLTLC(rgnode *state, atomicmeta &am) {
    if(am.groundtruth == TRUE)
        return true;
    else if(am.groundtruth == FALSE)
        return false;

    unsigned int left = 0;
    unsigned int right = 0;
    const cardmeta *p;

    //left
    if (am.leftexp.constnum != -1)
        left = am.leftexp.constnum;
    else {
        p = am.leftexp.expression;
        while (p) {
            left += state->readPlace(p->placeid);
            p = p->next;
        }
    }

    //right
    if (am.rightexp.constnum != -1)
        right = am.rightexp.constnum;
    else {
        p = am.rightexp.expression;
        while (p) {
            right += state->readPlace(p->placeid);
            p = p->next;
        }
    }

    am.last_check_avaliable = false;
    am.last_check = left <= right;
    return left <= right;
}

template<class rgnode, class rg_T>
double Product_Automata<rgnode, rg_T>::get_current_mem() {
    double size=0;
    char filename[64];
    sprintf(filename,"/proc/%d/status",mypid);
    FILE *pf = fopen(filename,"r");
    if(pf == nullptr)
    {
        cerr<<"未能检测到enPAC进程所占内存"<<endl;
        pclose(pf);
    }
    else {
        char line[128];
        while(fgets(line,128,pf) != nullptr)
        {
            if(strncmp(line,"VmRSS:",6) == 0)
            {
                int len = strlen(line);
                const char *p=line;
                for(;std::isdigit(*p) == false;++p) {}
                line[len-3]=0;
                size = atoi(p);
                break;
            }
        }
        fclose(pf);
        size = size/1024.0;
    }
    return size;
}
#endif //ENPAC_2020_2_0_PRODUCT_H
