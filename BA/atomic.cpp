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
    if(this->constnum!=MAXUNSHORT16 && exp2.constnum!=MAXUNSHORT16) {
        this->constnum -= exp2.constnum;
    }
    else if(this->constnum==MAXUNSHORT16 && exp2.constnum==MAXUNSHORT16) {
        cardmeta *sub = exp2.expression;
        cardmeta *location;
        while (sub) {
            location=this->locate(sub->placeid);
            if(location!=NULL) {
                location->coefficient -= sub->coefficient;
                if(location->coefficient == 0) {
                    this->remove(sub->placeid);
                }
            } else {
                cardmeta *meta = new cardmeta;
                meta->placeid = sub->placeid;
                meta->coefficient = -1*(sub->coefficient);
                this->insert(meta);
            }
            sub=sub->next;
        }
    }
    else {
        cerr<<"[Error@cardexp::MINUS] can not minus."<<endl;
    }
}

void cardexp::PLUS(const cardexp &exp2) {
    if(this->constnum!=MAXUNSHORT16 && exp2.constnum!=MAXUNSHORT16) {
        this->constnum += exp2.constnum;
    }
    else if(this->constnum==MAXUNSHORT16 && exp2.constnum==MAXUNSHORT16) {
        cardmeta *sub = exp2.expression;
        cardmeta *location;
        while (sub) {
            location=this->locate(sub->placeid);
            if(location!=NULL) {
                location->coefficient += sub->coefficient;
                if(location->coefficient == 0) {
                    this->remove(sub->placeid);
                }
            } else {
                cardmeta *meta = new cardmeta;
                meta->placeid = sub->placeid;
                meta->coefficient = sub->coefficient;
                this->insert(meta);
            }
            sub=sub->next;
        }
    }
    else {
        cerr<<"[Error@cardexp::MINUS] can not minus."<<endl;
    }
}

cardmeta *cardexp::locate(unsigned int placeid) {
    cardmeta *p = expression;
    while (p) {
        if(p->placeid == placeid)
            return p;
        p=p->next;
    }
    return NULL;
}

void cardexp::insert(cardmeta *meta) {
    /*头插法*/
    meta->next = expression;
    expression = meta;
//    cardmeta *p=expression->next,*q=expression;
//    while (p) {
//        if(p->placeid > meta->placeid)
//            break;
//        q=p;
//        p=p->next;
//    }
//    meta->next = q->next;
//    q->next = meta;
}

void cardexp::DestroyExp() {
    cardmeta *p,*q;
    p=expression;
    while (p) {
        q=p->next;
        delete p;
        p=q;
    }
    expression = NULL;
}

int cardexp::placenum() {
    if(constnum!=MAXUNSHORT16)
        return 0;
    cardmeta *p = expression;
    int i=0;
    for(i,p;p!=NULL;p=p->next,++i);
    return i;
}

int cardexp::unitnum() {
    if(constnum!=MAXUNSHORT16)
        return 0;
    set<index_t> unitset;
    cardmeta *p = expression;
    while (p) {
        unitset.insert(petri->nupnExtra[p->placeid].myunit);
        p=p->next;
    }
    return unitset.size();
}

void cardexp::remove(unsigned int placeid) {
    if(expression->placeid == placeid) {
        cardmeta *p = expression;
        expression = expression->next;
        delete p;
        return;
    }
    cardmeta *meta = expression->next;
    cardmeta *pre = expression;
    while(meta!=NULL) {
        if(meta->placeid == placeid) {
            pre->next=meta->next;
            delete meta;
            return;
        }
        pre = meta;
        meta = meta->next;
    }
}

bool cardexp::semi_positive() {
    bool semi_positive = true;
    cardmeta *meta = expression;
    while (meta){
        if(meta->coefficient<0) {
            semi_positive = false;
            break;
        }
        meta = meta->next;
    }
    return semi_positive;
}

void cardexp::SCALAR(int factor) {
    if(this->constnum == MAXUNSHORT16) {
        cardmeta *meta = expression;
        while (meta) {
            meta->coefficient *= factor;
            meta = meta->next;
        }
    }
    else {
        this->constnum * factor;
    }
}

bool cardexp::semi_negative() {
    bool semi_negative = true;
    cardmeta *meta = expression;
    while (meta){
        if(meta->coefficient>0) {
            semi_negative = false;
            break;
        }
        meta = meta->next;
    }
    return semi_negative;
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


int atomicmeta::addPlace2Exp(bool left, const string &placeName) {
    cardexp *exp = left ? &leftexp : &rightexp;
    if (exp->constnum != MAXUNSHORT16) {
        cerr << "Error, try to add place to integer-constant type" << endl;
        exit(-1);
    }
    index_t idx_P = petri->getPPosition(placeName);
    if (idx_P == INDEX_ERROR) {
        return CONSISTENCY_ERROR;
//        cerr << "Error in locate place '" << placeName << "'!" << endl;
//        exit(-1);
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
    return OK;
}

void atomicmeta::evaluate() {
    if (mytype == PT_CARDINALITY) {
        if (rightexp.semi_positive() && leftexp.constnum <= 0) {
            groundtruth = TRUE;
            return;
        }
        if (petri->SAFE) {
            if (rightexp.semi_positive() && leftexp.constnum > rightexp.placenum()) {
                groundtruth = FALSE;
                return;
            }
            if (rightexp.semi_negative() && rightexp.placenum() <= (-1 * leftexp.constnum)) {
                groundtruth = TRUE;
                return;
            }
        }
        if (petri->NUPN) {
            if (rightexp.semi_positive() && leftexp.constnum > rightexp.unitnum()) {
                groundtruth = FALSE;
                return;
            }
            if (rightexp.semi_negative() && rightexp.unitnum() <= (-1 * leftexp.constnum)) {
                groundtruth = TRUE;
                return;
            }
        }
        if (petri->PINVAR) {
            int *coefficients = new int[placecount];
            memset(coefficients, 0, sizeof(int) * placecount);
            if (leftexp.constnum == MAXUNSHORT16 || rightexp.constnum != MAXUNSHORT16) {
                cerr<< "[Error@atomicmeta::evaluate] LTLCardinality atomic propositions have not translated into unified form yet."<< endl;
            }
            int k = leftexp.constnum;
            cardmeta *meta = rightexp.expression;
            while (meta) {
                coefficients[meta->placeid] = meta->coefficient;
                meta = meta->next;
            }

            for (int i = 0; i < placecount - petri->RankOfmatrix; i++) {
                //判断当前P不变量系数是否都大于或者都小于coefficients
                bool geq = true, leq = true;
                for (int j = 0; j < placecount; j++) {
                    if (petri->Pinvar[i][j] > coefficients[j]) {
                        leq = false;
                        break;
                    }
                }
                for (int j = 0; j < placecount; j++) {
                    if (petri->Pinvar[i][j] < coefficients[j]) {
                        geq = false;
                        break;
                    }
                }
                if (geq && petri->weightsum0[i] < k) {
                    groundtruth = FALSE;
                    break;
                }
                if (leq && petri->weightsum0[i] >= k) {
                    groundtruth = TRUE;
                    break;
                }
            }
            delete [] coefficients;
        }
    }
}

/*Unify the form to k<=(k1p1+k2p2+...)-(kipi+k_(i+1)p_(i+1)+...) by shifting items*/
void atomicmeta::tranpose() {
    if(leftexp.constnum==MAXUNSHORT16 && rightexp.constnum==MAXUNSHORT16 ) {
        /*左右两边都为k1p1+k2p2+...的形式*/
        rightexp.MINUS(leftexp);
        leftexp.constnum = 0;
        leftexp.DestroyExp();
    }
    else if(leftexp.constnum==MAXUNSHORT16 && rightexp.constnum!=MAXUNSHORT16) {
        /*左边为k1p1+k2p2+...的形式，右边为一个常数*/
        leftexp.SCALAR(-1);
        rightexp.expression = leftexp.expression;
        leftexp.expression = NULL;
        leftexp.constnum = -1 * rightexp.constnum;
        rightexp.constnum = MAXUNSHORT16;
    }
    else if(leftexp.constnum!=MAXUNSHORT16 && rightexp.constnum==MAXUNSHORT16) {
        /*左边为常数，右边为k1p1+k2p2+...的形式*/
    }
    else if(leftexp.constnum!=MAXUNSHORT16 && rightexp.constnum!=MAXUNSHORT16) {
        rightexp.constnum -= leftexp.constnum;
        leftexp.constnum = 0;
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
            if (atomics[atomiccount].leftexp.constnum != MAXUNSHORT16)
                atomics[atomiccount].leftexp.constnum = MAXUNSHORT16;
            else {
                cardmeta *exp = atomics[atomiccount].leftexp.expression, *temp;
                while (exp) {
                    temp = exp->next;
                    delete exp;
                    exp = temp;
                }
                atomics[atomiccount].leftexp.expression = NULL;
            }

            if (atomics[atomiccount].rightexp.constnum != MAXUNSHORT16)
                atomics[atomiccount].rightexp.constnum = MAXUNSHORT16;
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

                if (atomics[k].leftexp.constnum == MAXUNSHORT16) { // if const num is not -1 then pass
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
                if (atomics[k].rightexp.constnum == MAXUNSHORT16) {  // right
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