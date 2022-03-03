//
// Created by hecong on 2020/9/28.
//

#ifndef ENPAC_2021_ATOMIC_H
#define ENPAC_2021_ATOMIC_H

#include <iostream>
#include <string>
#include <set>
#include "Petri_Net.h"

using namespace std;

#define OK 1
#define CONSISTENCY_ERROR 0
#define ATOMICTABLESIZE 50

extern Petri *petri;

enum AtomicType{PT_CARDINALITY,PT_FIREABILITY};
enum Evaluation{UNKNOW,TRUE,FALSE};

typedef struct cardmeta {
    short coefficient = 1;
    unsigned int placeid;
    cardmeta *next = NULL;
} cardmeta;

class cardexp {
public:
    int constnum;
    cardmeta *expression;

    cardexp(){constnum=-1;expression = NULL;/*expression=new cardmeta;*/}
    ~cardexp();
    void DestroyExp();
    cardmeta *locate(unsigned int placeid);
    int placenum();
    int unitnum();
    void insert(cardmeta *meta);
    void remove(unsigned int placeid);
    void MINUS (const cardexp &exp2);
    void PLUS (const cardexp &exp2);
    void SCALAR (int factor);
    bool semi_positive();
};

class atomicmeta
{
public:
    AtomicType mytype;
    string mystr;
    cardexp leftexp;
    cardexp rightexp;
    vector<unsigned int> fires;
    Evaluation groundtruth;

    bool last_check;
    bool last_check_avaliable;

public:
    atomicmeta(){last_check_avaliable=false;groundtruth=UNKNOW;}

    int addPlace2Exp(bool left, const string &placeName);

    /*Unify the form to k<=(k1p1+k2p2+...)-(kipi+k_(i+1)p_(i+1)+...) by shifting items*/
    void tranpose();

    void evaluate();
    int parse();
    int parse_card();
    int parse_fire();
};

typedef struct atomic {
    bool negation;
    unsigned short atomicmeta_link;
} atomic, Atomic;

class atomictable
{
public:
    atomicmeta atomics[ATOMICTABLESIZE];
    int atomiccount;
public:
    atomictable(){atomiccount = 0;}//init, atomics[0] always empty

    void checkRepeat();//check the latest one. If repeated, delete and release memory.
    void linkPlace2atomictable();    //构建库所影响的原子命题序列
};
#endif //ENPAC_2021_ATOMIC_H
