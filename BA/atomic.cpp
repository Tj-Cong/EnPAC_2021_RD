//
// Created by hecong on 2020/9/28.
//
#include "atomic.h"
#include <algorithm>

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
        fires.push_back(idex);
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


void atomicmeta::addPlace2Exp(bool left, const string &placeName) {
    cardexp *exp = left ? &leftexp : &rightexp;
    if (exp->constnum != -1) {
        cerr << "Error, try to add place to integer-constant type" << endl;
        exit(-1);
    }
    index_t idx_P = petri->getPPosition(placeName);
    if (idx_P == INDEX_ERROR) {
        cerr << "Error in locate place '" << placeName << "'!" << endl;
        exit(-1);
    }
    else {
        cardmeta *meta = exp->expression;
        if (!meta) {     //first cardmeta
            exp->expression = new cardmeta;
            meta = exp->expression;
        } else {
            while (meta->next)
                meta = meta->next;
            meta->next = new cardmeta;
            meta = meta->next;
        }
        meta->placeid = idx_P;
        meta->coefficient = 1;
    }
}

void atomictable::checkRepeat() {
    int i;
    for (i = 1; i < atomiccount; i++)//locate repeated atomicmeta
        if (atomics[atomiccount].mystr == atomics[i].mystr)
            break;

    if (i != atomiccount) {//exist
        atomics[atomiccount].mystr.clear();
        //Type doesn't need to be changed
        if (atomics[atomiccount].mytype == PT_CARDINALITY) {//card, handle left and right
            if (atomics[atomiccount].leftexp.constnum != -1)
                atomics[atomiccount].leftexp.constnum = -1;
            else {
                cardmeta *exp = atomics[atomiccount].leftexp.expression, *temp;
                while (exp) {
                    temp = exp->next;
                    delete exp;
                    exp = temp;
                }
                atomics[atomiccount].leftexp.expression = NULL;
            }

            if (atomics[atomiccount].rightexp.constnum != -1)
                atomics[atomiccount].rightexp.constnum = -1;
            else {
                cardmeta *exp = atomics[atomiccount].rightexp.expression, *temp;
                while (exp) {
                    temp = exp->next;
                    delete exp;
                    exp = temp;
                }
                atomics[atomiccount].rightexp.expression = NULL;
            }

        }
        else if (atomics[atomiccount].mytype == PT_FIREABILITY) {//fire
            atomics[atomiccount].fires.clear();
        }
        else{
            cerr << "Error, atomicmeta type error while checking repeat!" << endl;
            exit(-1);
        }

        atomiccount--;
    }
}

/* 构建库所影响的原子命题序列
 * 不可以构建成const形式
 * */
void atomictable::linkPlace2atomictable() {
    if (atomiccount == 0)
        return;             // No atomic existing
    AtomicType LTL_TYPE = atomics[atomiccount].mytype;
    int i, j, k;

    for (i = 0; i < petri->placecount; i++) {
        petri->place[i].atomicLinks.clear();  // clear last formula

        if (LTL_TYPE == PT_FIREABILITY) { //Firability
            int t_num = petri->place[i].consumer.size();  // num of transition

            for (j = 0; j < t_num; j++) {
                index_t t_idx = petri->place[i].consumer[j].idx;  // transition index

                for (k = 1; k <= atomiccount; k++) {
                    vector<index_t>::iterator it = find(atomics[k].fires.begin(), atomics[k].fires.end(),
                                                        t_idx); //try to find consumer in atomics[k]
                    //
                    if (it != atomics[k].fires.end()) {  // existing
                        petri->place[i].atomicLinks.push_back(k);  // add to link table

                    }
                }
            }
        }
        else { //Cardinality  if(LTL_TYPE == PT_CARDINALITY)
            for (k = 0; k <= atomiccount; k++) { // check every atomicmeta

                if (atomics[k].leftexp.constnum == -1) { // if const num is not -1 then pass
                    cardmeta *p = atomics[k].leftexp.expression;
                    while (p) {
                        if (p->placeid == i) {
                            petri->place[i].atomicLinks.push_back(k);
                            break;  // appear once
                        }
                        p = p->next;
                    }
                    if (!petri->place[i].atomicLinks.empty())
                        continue;// continue to prevent 2 same values in atomicLinks
                }
                if (atomics[k].rightexp.constnum == -1) {  // right
                    cardmeta *p = atomics[k].rightexp.expression;
                    while (p) {
                        if (p->placeid == i) {
                            petri->place[i].atomicLinks.push_back(k);
                            break;  // appear once
                        }
                        p = p->next;
                    }
                }

            }
        }

        petri->place[i].atomicLinks.shrink_to_fit();  // release memory
    }
}