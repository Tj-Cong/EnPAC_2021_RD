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

typedef struct cardmeta {
    short coefficient = 1;
    unsigned int placeid;
    cardmeta *next = NULL;
} cardmeta;

class cardexp {
public:
    int constnum;
    cardmeta *expression;

    cardexp(){constnum=-1;expression=new cardmeta;}
    ~cardexp();
    void DestroyExp();
    cardmeta *locate(unsigned int placeid);
    void insert(cardmeta *meta);
    void MINUS (const cardexp &exp2);
};

class atomicmeta
{
public:
    AtomicType mytype;
    string mystr;
    cardexp leftexp;
    cardexp rightexp;
    set<unsigned int> fires;

    int parse();
    int parse_card();
    int parse_fire();
    void transform();
};

typedef struct atomic
{
    bool negation;
    unsigned short atomicmeta_link;
};

class atomictable
{
    atomicmeta atomics[ATOMICTABLESIZE];
    int atomiccount;
};
#endif //ENPAC_2021_ATOMIC_H
