//
// Created by hecong on 2020/9/28.
//
#include "atomic.h"

cardexp::~cardexp(){
    cardmeta *p,*q;
    p=expression;
    while (p) {
        q=p->next;
        delete p;
        p=q;
    }
}

void cardexp::MINUS(const cardexp &exp2) {
    cardmeta *sub = exp2.expression->next;
    cardmeta *location;
    while (sub) {
        location=this->locate(sub->placeid);
        if(location!=NULL) {
            location->coefficient -= sub->coefficient;
        } else {
            cardmeta *meta = new cardmeta;
            meta->placeid = sub->placeid;
            meta->coefficient = -1*(sub->coefficient);
            this->insert(meta);
        }
        sub=sub->next;
    }
}

cardmeta *cardexp::locate(unsigned int placeid) {
    cardmeta *p = expression->next;
    while (p) {
        if(p->placeid == placeid)
            return p;
        p=p->next;
    }
    return NULL;
}

void cardexp::insert(cardmeta *meta) {
    cardmeta *p=expression->next,*q=expression;
    while (p) {
        if(p->placeid > meta->placeid)
            break;
        q=p;
        p=p->next;
    }
    meta->next = q->next;
    q->next = meta;
}

void cardexp::DestroyExp() {
    cardmeta *p,*q;
    p=expression;
    while (p) {
        q=p->next;
        delete p;
        p=q;
    }
};

int atomicmeta::parse() {
    int pos = mystr.find("<=");
    if (pos == string::npos)
    {
        mytype = PT_FIREABILITY;            //LTLFireability category
        return parse_fire();
    }
    else {
        mytype = PT_CARDINALITY;
        return parse_card();
    }
}

int atomicmeta::parse_card() {
    int pos = mystr.find_first_of("<=");              //定位到<=前
    string leftplaces = mystr.substr(1,pos-1);   //去除"token-count(" ")"  ֻ只剩p1,p2,
    string rightplaces = mystr.substr(pos+2,mystr.length()-pos-3);
    if(leftplaces[0] == 't')  //前半部分是token-count的形式
    {
        leftplaces = leftplaces.substr(12);
        while (1) {
            int commapos = leftplaces.find_first_of(",");
            if(commapos==string::npos)
                break;
            string placename = leftplaces.substr(0,pos);
            index_t idex = petri->getPPosition(placename);  //得到该库所的索引号
            if (idex == INDEX_ERROR) {
                return CONSISTENCY_ERROR;
            }
            cardmeta *p = new cardmeta;
            p->coefficient=1;
            p->placeid = idex;
            leftexp.insert(p);
            //将前面的用过的P1去除 从p2开始作为新的s串
            leftplaces = leftplaces.substr(commapos + 1);
        }
    }
    else {
        leftexp.constnum = atoi(leftplaces.c_str());
    }

    if(rightplaces[0]=='t')
    {
        rightplaces = rightplaces.substr(12);
        while(1) {
            int commapos = rightplaces.find_first_of(",");
            if(commapos == string::npos)
                break;
            string placename = rightplaces.substr(0,commapos);
            index_t idex = petri->getPPosition(placename);  //得到该库所的索引号
            if (idex == INDEX_ERROR) {
                return CONSISTENCY_ERROR;
            }
            cardmeta *p=new cardmeta;
            p->coefficient = 1;
            p->placeid = idex;
            rightexp.insert(p);
            rightplaces=rightplaces.substr(commapos+1);
        }
    }
    else {
        rightexp.constnum = atoi(rightplaces.c_str());
    }

    return OK;
}

int atomicmeta::parse_fire() {
    string s=mystr.substr(1,mystr.length()-2);    //delete '{' and '}'
    while (1) {
        int pos = s.find_first_of(",");
        if (pos < 0)
            break;
        string subs = s.substr(0, pos);  //取出一个变迁
        int idex = petri->getTPosition(subs);  //变迁的序列号
        if(idex == INDEX_ERROR){
            return CONSISTENCY_ERROR;
        }
        fires.insert(idex);
        s = s.substr(pos + 1, s.length() - pos);
    }
    return OK;
}

void atomicmeta::transform() {
    if(leftexp.constnum==-1 && rightexp.constnum==-1 ) {
        rightexp.MINUS(leftexp);
        leftexp.constnum = 0;
        leftexp.DestroyExp();
    }
}
