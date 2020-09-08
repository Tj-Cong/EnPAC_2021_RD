//
// Created by lewis on 19-7-21.
//

#ifndef ENPAC_2020_3_0_RG_H
#define ENPAC_2020_3_0_RG_H

#include "Petri_Net.h"
#include <bitset>
#include <malloc.h>
#include <gperftools/malloc_extension.h>
#include "BA/set.h"

using namespace std;

#define RGTABLE_SIZE 1048576
#define SHORTMAX 0xffff

extern NUM_t FIELDCOUNT;   //占用bitfield个数，仅仅用于NUPN和SAFE网
extern NUM_t MARKLEN;      //Petri网
extern NUM_t placecount;   //Petri网库所个数
extern bool NUPN;          //当前Petri网是否有NUPN信息
extern bool SAFE;          //当前Petri网是否为安全网
extern Petri *petri;

void setGlobalValue(Petri *ptnet);

typedef struct Bitfielduint {
    unsigned int a0:1;
    unsigned int a1:1;
    unsigned int a2:1;
    unsigned int a3:1;
    unsigned int a4:1;
    unsigned int a5:1;
    unsigned int a6:1;
    unsigned int a7:1;
    unsigned int a8:1;
    unsigned int a9:1;
    unsigned int a10:1;
    unsigned int a11:1;
    unsigned int a12:1;
    unsigned int a13:1;
    unsigned int a14:1;
    unsigned int a15:1;
    unsigned int a16:1;
    unsigned int a17:1;
    unsigned int a18:1;
    unsigned int a19:1;
    unsigned int a20:1;
    unsigned int a21:1;
    unsigned int a22:1;
    unsigned int a23:1;
    unsigned int a24:1;
    unsigned int a25:1;
    unsigned int a26:1;
    unsigned int a27:1;
    unsigned int a28:1;
    unsigned int a29:1;
    unsigned int a30:1;
    unsigned int a31:1;

    //将相应位置上设置为1；
    void set(int index);     //设置index位上为1
    void reset(int index);   //将index位置上设置为0；
    bool test0(int index);   //判断index位上是否为0；
    bool test1(int index);   //判断index位上是否为1；
} myuint;


/*********************Global Functions**********************/
void DecToBinary(index_t DecNum, unsigned short *Binarystr);

void DecToBinaryWithDigit(index_t DecNum, unsigned short *Binarystr, unsigned short digit);

void BinaryToDecWithRedundancy(index_t &DecNum, unsigned short *Binarystr, NUM_t marklen);

void BinaryToDec(index_t &DecNum, unsigned short *Binarystr, NUM_t marklen);

typedef unsigned short Mark;
typedef unsigned long ID;


class RGNode {
public:
    Mark *marking;
    RGNode *next;
    set<index_t> fireset;
public:
    RGNode();
    index_t Hash();
    bool isFirable(const Transition &t) const;
    void getFireSet(RGNode *lastnode, index_t lastid);
    void printMarking(const int &len);
    ~RGNode();
    bool readPlace(int placeid) const {};
    void writePlace(int placeid){};
    void clearPlace(int placeid){};
};

//用于SAFE和NUPN
class BitRGNode {
public:
    myuint *marking;
    BitRGNode *next;
    set<index_t> fireset;
public:
    BitRGNode();
    index_t Hash();
    bool isFirable(const Transition &t) const;
    void getFireSet(BitRGNode *lastnode, index_t lastid);
    void printMarking(const int &len);
    ~BitRGNode();
    bool readPlace(int placeid) const;
    void writePlace(int placeid);
    void clearPlace(int placeid);
};

class RG {
public:
    RGNode **rgnode;        //状态哈希表
    Petri *ptnet;           //Petri网
    RGNode *initnode;       //初始状态节点
    NUM_t RGNodelength;     //marking长度
    unsigned long nodecount;//状态个数
    int hash_conflict_times;//哈希冲突次数
    ofstream outRG;
public:
    RG(Petri *pt);

    void addRGNode(RGNode *mark);

    index_t getHashIndex(RGNode *mark);

    RGNode *RGinitialnode();

    RGNode *RGcreatenode(RGNode *curnode, int tranxnum, bool &exist);

    void getFireableTranx(RGNode *curnode, set<index_t> &fireset);

    void Generate(RGNode *node);

//    void printRGNode(RGNode *node);

    ~RG();
};

class BitRG {
public:
    BitRGNode **rgnode;
    Petri *ptnet;
    BitRGNode *initnode;
    NUM_t RGNodelength;
    unsigned long nodecount;
    int hash_conflict_times;
    ofstream outRG;
public:
    BitRG(Petri *pt);

    void addRGNode(BitRGNode *mark);

    index_t getHashIndex(BitRGNode *mark);

    BitRGNode *RGinitialnode();

    BitRGNode *RGcreatenode(BitRGNode *curnode, int tranxnum, bool &exist);

    void getFireableTranx(BitRGNode *curnode,set<index_t> &fireset);

    void Generate(BitRGNode *node);

//    void printRGNode(BitRGNode *node);

    ~BitRG();
};

#endif //ENPAC_2020_3_0_RG_H
