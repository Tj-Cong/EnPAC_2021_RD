//
// Created by lewis on 19-7-19.
//

#ifndef ENPAC_2020_3_0_PETRI_NET_H
#define ENPAC_2020_3_0_PETRI_NET_H

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include "BA/tinyxml.h"
#include <cmath>
#include <gperftools/tcmalloc.h>
#include <set>
#include <map>
#include <csetjmp>

using namespace std;

#define INDEX_ERROR 0xffffffff

typedef unsigned int index_t;    //索引数据类型
typedef unsigned short weight_t; //弧权重的数据类型
typedef unsigned short initM_t;  //初始token的数据类型
typedef unsigned int NUM_t;      //个数数据类型

extern NUM_t FIELDCOUNT;   //占用bitfield个数，仅仅用于NUPN和SAFE网
extern NUM_t MARKLEN;      //Petri网
extern NUM_t placecount;   //Petri网库所个数
extern bool NUPN;          //当前Petri网是否有NUPN信息
extern bool SAFE;          //当前Petri网是否为安全网

struct Small_Arc;
struct Transition;

/****************Global Functions**************/
unsigned int BKDRHash(string str);

unsigned int stringToNum(const string &str);

void intersection(const vector<Small_Arc> &t1pre, const vector<Small_Arc> &t2pre, vector<int> &secidx);

int my_atoi(string str);

/**********************************************/

struct Unit {        //每一个单元的信息
    string uid = "";
    NUM_t size;     //该unit中库所的个数;
    NUM_t mark_length;  //该unit的marking长度 ceil
    index_t startpos;   //该单元第一个库所的索引位置
    index_t mark_sp;    //该unit的marking在实际marking中的一个位置
};
typedef struct Small_Arc {
    weight_t weight;  //该弧上的权重
    index_t idx;

    bool operator==(const Small_Arc &b) {
        if (this->weight == b.weight && this->idx == b.idx)
            return true;
        else
            return false;
    }
} SArc;

typedef struct Place {
    string id = "";
    vector<SArc> producer;       //前继
    vector<SArc> consumer;       //后继
    unsigned int initialMarking = 0;  //初值token
    index_t myunit;              //该库所所在单元号
    index_t myoffset;            //该库所在单元中的偏移量
} *Place_P;

typedef struct Place_extra {
    bool cutoff = false;
    int intnum;
    int intoffset;
    index_t low_read_mask=0;
    index_t high_read_mask=0;
    index_t low_zero_mask=0;
    index_t high_zero_mask=0;
    index_t low_write_mask=0;
    index_t high_write_mask=0;
} Place_extra;

typedef struct Transition {
    string id = "";
    vector<SArc> producer;
    vector<SArc> consumer;
    set<index_t> increasing;
    set<index_t> decreasing;
} *Transition_P;

typedef struct Arc {
    string id = "";
    bool isp2t;
    string source_id;
    string target_id;
    int source_idx;
    int target_idx;
    index_t weight = 1;
} *Arc_P;

class Petri {
public:
    map<string,index_t> mapPlace;
    map<string,index_t> mapTransition;
    Place *place;               //库所表
    Transition *transition;     //变迁表
    Arc *arc;                   //弧表
    Unit *unittable;            //单元表
    Place_extra *placeExtra;
    NUM_t placecount;           //库所个数
    NUM_t transitioncount;      //变迁个数
    NUM_t arccount;             //弧个数
    NUM_t unitcount;            //单元个数
    bool NUPN;
    bool SAFE;
public:
    Petri();

    void getSize(char *filename);                   //预处理，得到库所，变迁，弧个数，如果是NUPN,会调用preNUPN进行NUPN的预处理
    void preNUPN(TiXmlElement *structure);          //NUPN的预处理
    void readNUPN(char *filename);                  //NUPN网的解析
    NUM_t getPlaceSize() const;
    void allocHashTable();                          //申请空间
    index_t getPPosition(string str);               //根据库所id得到他在库所表中的索引位置
    index_t getTPosition(string str);               //根据变迁id得到他在变迁表中的索引位置
    index_t getPosition(string str, bool &isplace); //根据id得到他相应所表中的索引位置，并指明是库所还是变迁
    void readPNML(char *filename);                  //第二次解析PNML
    void computeUnitMarkLen();                      //计算每一个unit的marking长度
    void judgeSAFE();
    void checkarc();
    void computeDI();
    void printPlace();
    void printTransition();
    void printTransition2CSV();
    void printGraph();
    void printUnit();

    ~Petri();
};

#endif //ENPAC_2020_3_0_PETRI_NET_H
