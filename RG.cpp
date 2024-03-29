//
// Created by lewis on 19-7-21.
//
#include "RG.h"

/*********************Global Functions**********************/
void DecToBinaryWithDigit(index_t DecNum, unsigned short *Binarystr, unsigned short digit) {
    int i = 0;
    while (DecNum) {
        Binarystr[i++] = DecNum % 2;
        DecNum = DecNum / 2;
    }
    //剩余的位置填0
    while (digit > i) {
        Binarystr[i++] = 0;
    }
}

void BinaryToDecWithRedundancy(index_t &DecNum, unsigned short *Binarystr, NUM_t marklen) {
    DecNum = 0;
    for (int i = marklen - 1; i >= 0; i--) {
        DecNum += Binarystr[i] * pow(2, marklen - i - 1);
    }
}

void DecToBinary(index_t DecNum, unsigned short *Binarystr) {
    int i = 0;
    while (DecNum) {
        Binarystr[i++] = DecNum % 2;
        DecNum = DecNum / 2;
    }
}

void BinaryToDec(index_t &DecNum, unsigned short *Binarystr, NUM_t marklen) {
    int i = marklen - 1;
    DecNum = 0;
    for (i; i >= 0; i--) {
        DecNum = DecNum * 2 + Binarystr[i];
    }
}

void setGlobalValue(Petri *ptnet) {
    MARKLEN = 0;
    if (NUPN) {
        for (int i = 0; i < ptnet->unitcount; i++) {
            MARKLEN += ptnet->unittable[i].mark_length;
        }
    } else if (SAFE) {
        MARKLEN = ptnet->placecount;
    } else if (PINVAR && SLICEPLACE) {
        for(int i=0;i<placecount;++i) {
            if(ptnet->sliceExtra[i].significant)
                MARKLEN += ptnet->pinvarSliceExtra[i].length;
        }
    } else if (PINVAR) {
        for(int i=0;i<placecount;++i) {
            if(ptnet->pinvarExtra[i].significant)
                MARKLEN += ptnet->pinvarExtra[i].length;
        }
    } else if (LONGBITPLACE) {
        MARKLEN = 32*(ptnet->slicePlaceCount);
    } else {
        MARKLEN = ptnet->slicePlaceCount;
    }
    FIELDCOUNT = ceil(double(MARKLEN) / (sizeof(myuint) * 8));
}

void Bitfielduint::set(int index) {
    switch (index) {
        case 0: {
            a0 = 1;
            break;
        }
        case 1: {
            a1 = 1;
            break;
        }
        case 2: {
            a2 = 1;
            break;
        }
        case 3: {
            a3 = 1;
            break;
        }
        case 4: {
            a4 = 1;
            break;
        }
        case 5: {
            a5 = 1;
            break;
        }
        case 6: {
            a6 = 1;
            break;
        }
        case 7: {
            a7 = 1;
            break;
        }
        case 8: {
            a8 = 1;
            break;
        }
        case 9: {
            a9 = 1;
            break;
        }
        case 10: {
            a10 = 1;
            break;
        }
        case 11: {
            a11 = 1;
            break;
        }
        case 12: {
            a12 = 1;
            break;
        }
        case 13: {
            a13 = 1;
            break;
        }
        case 14: {
            a14 = 1;
            break;
        }
        case 15: {
            a15 = 1;
            break;
        }
        case 16: {
            a16 = 1;
            break;
        }
        case 17: {
            a17 = 1;
            break;
        }
        case 18: {
            a18 = 1;
            break;
        }
        case 19: {
            a19 = 1;
            break;
        }
        case 20: {
            a20 = 1;
            break;
        }
        case 21: {
            a21 = 1;
            break;
        }
        case 22: {
            a22 = 1;
            break;
        }
        case 23: {
            a23 = 1;
            break;
        }
        case 24: {
            a24 = 1;
            break;
        }
        case 25: {
            a25 = 1;
            break;
        }
        case 26: {
            a26 = 1;
            break;
        }
        case 27: {
            a27 = 1;
            break;
        }
        case 28: {
            a28 = 1;
            break;
        }
        case 29: {
            a29 = 1;
            break;
        }
        case 30: {
            a30 = 1;
            break;
        }
        case 31: {
            a31 = 1;
            break;
        }
        default: {
            cerr << "Bitfield wrong index" << endl;
            exit(0);
        }
    }
}

void Bitfielduint::reset(int index) {
    switch (index) {
        case 0: {
            a0 = 0;
            break;
        }
        case 1: {
            a1 = 0;
            break;
        }
        case 2: {
            a2 = 0;
            break;
        }
        case 3: {
            a3 = 0;
            break;
        }
        case 4: {
            a4 = 0;
            break;
        }
        case 5: {
            a5 = 0;
            break;
        }
        case 6: {
            a6 = 0;
            break;
        }
        case 7: {
            a7 = 0;
            break;
        }
        case 8: {
            a8 = 0;
            break;
        }
        case 9: {
            a9 = 0;
            break;
        }
        case 10: {
            a10 = 0;
            break;
        }
        case 11: {
            a11 = 0;
            break;
        }
        case 12: {
            a12 = 0;
            break;
        }
        case 13: {
            a13 = 0;
            break;
        }
        case 14: {
            a14 = 0;
            break;
        }
        case 15: {
            a15 = 0;
            break;
        }
        case 16: {
            a16 = 0;
            break;
        }
        case 17: {
            a17 = 0;
            break;
        }
        case 18: {
            a18 = 0;
            break;
        }
        case 19: {
            a19 = 0;
            break;
        }
        case 20: {
            a20 = 0;
            break;
        }
        case 21: {
            a21 = 0;
            break;
        }
        case 22: {
            a22 = 0;
            break;
        }
        case 23: {
            a23 = 0;
            break;
        }
        case 24: {
            a24 = 0;
            break;
        }
        case 25: {
            a25 = 0;
            break;
        }
        case 26: {
            a26 = 0;
            break;
        }
        case 27: {
            a27 = 0;
            break;
        }
        case 28: {
            a28 = 0;
            break;
        }
        case 29: {
            a29 = 0;
            break;
        }
        case 30: {
            a30 = 0;
            break;
        }
        case 31: {
            a31 = 0;
            break;
        }
        default: {
            cerr << "Bitfield wrong index" << endl;
            exit(0);
        }
    }
}

bool Bitfielduint::test0(int index) {
    switch (index) {
        case 0: {
            if (a0 == 0) {
                return true;
            } else {
                return false;
            }
        }
        case 1: {
            if (a1 == 0) {
                return true;
            } else {
                return false;
            }
        }
        case 2: {
            if (a2 == 0) {
                return true;
            } else {
                return false;
            }
        }
        case 3: {
            if (a3 == 0) {
                return true;
            } else {
                return false;
            }
        }
        case 4: {
            if (a4 == 0) {
                return true;
            } else {
                return false;
            }
        }
        case 5: {
            if (a5 == 0) {
                return true;
            } else {
                return false;
            }
        }
        case 6: {
            if (a6 == 0) {
                return true;
            } else {
                return false;
            }
        }
        case 7: {
            if (a7 == 0) {
                return true;
            } else {
                return false;
            }
        }
        case 8: {
            if (a8 == 0) {
                return true;
            } else {
                return false;
            }
        }
        case 9: {
            if (a9 == 0) {
                return true;
            } else {
                return false;
            }
        }
        case 10: {
            if (a10 == 0) {
                return true;
            } else {
                return false;
            }
        }
        case 11: {
            if (a11 == 0) {
                return true;
            } else {
                return false;
            }
        }
        case 12: {
            if (a12 == 0) {
                return true;
            } else {
                return false;
            }
        }
        case 13: {
            if (a13 == 0) {
                return true;
            } else {
                return false;
            }
        }
        case 14: {
            if (a14 == 0) {
                return true;
            } else {
                return false;
            }
        }
        case 15: {
            if (a15 == 0) {
                return true;
            } else {
                return false;
            }
        }
        case 16: {
            if (a16 == 0) {
                return true;
            } else {
                return false;
            }
        }
        case 17: {
            if (a17 == 0) {
                return true;
            } else {
                return false;
            }
        }
        case 18: {
            if (a18 == 0) {
                return true;
            } else {
                return false;
            }
        }
        case 19: {
            if (a19 == 0) {
                return true;
            } else {
                return false;
            }
        }
        case 20: {
            if (a20 == 0) {
                return true;
            } else {
                return false;
            }
        }
        case 21: {
            if (a21 == 0) {
                return true;
            } else {
                return false;
            }
        }
        case 22: {
            if (a22 == 0) {
                return true;
            } else {
                return false;
            }
        }
        case 23: {
            if (a23 == 0) {
                return true;
            } else {
                return false;
            }
        }
        case 24: {
            if (a24 == 0) {
                return true;
            } else {
                return false;
            }
        }
        case 25: {
            if (a25 == 0) {
                return true;
            } else {
                return false;
            }
        }
        case 26: {
            if (a26 == 0) {
                return true;
            } else {
                return false;
            }
        }
        case 27: {
            if (a27 == 0) {
                return true;
            } else {
                return false;
            }
        }
        case 28: {
            if (a28 == 0) {
                return true;
            } else {
                return false;
            }
        }
        case 29: {
            if (a29 == 0) {
                return true;
            } else {
                return false;
            }
        }
        case 30: {
            if (a30 == 0) {
                return true;
            } else {
                return false;
            }
        }
        case 31: {
            if (a31 == 0) {
                return true;
            } else {
                return false;
            }
        }
        default: {
            cerr << "Bitfield wrong index" << endl;
            exit(0);
        }
    }
}

bool Bitfielduint::test1(int index) {
    switch (index) {
        case 0: {
            if (a0 == 1) {
                return true;
            } else {
                return false;
            }
        }
        case 1: {
            if (a1 == 1) {
                return true;
            } else {
                return false;
            }
        }
        case 2: {
            if (a2 == 1) {
                return true;
            } else {
                return false;
            }
        }
        case 3: {
            if (a3 == 1) {
                return true;
            } else {
                return false;
            }
        }
        case 4: {
            if (a4 == 1) {
                return true;
            } else {
                return false;
            }
        }
        case 5: {
            if (a5 == 1) {
                return true;
            } else {
                return false;
            }
        }
        case 6: {
            if (a6 == 1) {
                return true;
            } else {
                return false;
            }
        }
        case 7: {
            if (a7 == 1) {
                return true;
            } else {
                return false;
            }
        }
        case 8: {
            if (a8 == 1) {
                return true;
            } else {
                return false;
            }
        }
        case 9: {
            if (a9 == 1) {
                return true;
            } else {
                return false;
            }
        }
        case 10: {
            if (a10 == 1) {
                return true;
            } else {
                return false;
            }
        }
        case 11: {
            if (a11 == 1) {
                return true;
            } else {
                return false;
            }
        }
        case 12: {
            if (a12 == 1) {
                return true;
            } else {
                return false;
            }
        }
        case 13: {
            if (a13 == 1) {
                return true;
            } else {
                return false;
            }
        }
        case 14: {
            if (a14 == 1) {
                return true;
            } else {
                return false;
            }
        }
        case 15: {
            if (a15 == 1) {
                return true;
            } else {
                return false;
            }
        }
        case 16: {
            if (a16 == 1) {
                return true;
            } else {
                return false;
            }
        }
        case 17: {
            if (a17 == 1) {
                return true;
            } else {
                return false;
            }
        }
        case 18: {
            if (a18 == 1) {
                return true;
            } else {
                return false;
            }
        }
        case 19: {
            if (a19 == 1) {
                return true;
            } else {
                return false;
            }
        }
        case 20: {
            if (a20 == 1) {
                return true;
            } else {
                return false;
            }
        }
        case 21: {
            if (a21 == 1) {
                return true;
            } else {
                return false;
            }
        }
        case 22: {
            if (a22 == 1) {
                return true;
            } else {
                return false;
            }
        }
        case 23: {
            if (a23 == 1) {
                return true;
            } else {
                return false;
            }
        }
        case 24: {
            if (a24 == 1) {
                return true;
            } else {
                return false;
            }
        }
        case 25: {
            if (a25 == 1) {
                return true;
            } else {
                return false;
            }
        }
        case 26: {
            if (a26 == 1) {
                return true;
            } else {
                return false;
            }
        }
        case 27: {
            if (a27 == 1) {
                return true;
            } else {
                return false;
            }
        }
        case 28: {
            if (a28 == 1) {
                return true;
            } else {
                return false;
            }
        }
        case 29: {
            if (a29 == 1) {
                return true;
            } else {
                return false;
            }
        }
        case 30: {
            if (a30 == 1) {
                return true;
            } else {
                return false;
            }
        }
        case 31: {
            if (a31 == 1) {
                return true;
            } else {
                return false;
            }
        }
        default: {
            cerr << "Bitfield wrong index" << endl;
            exit(0);
        }
    }
}

BitSequence::BitSequence(int length) {
    int unitNums = ceil((float)length/32.0);
    bitUnits = new myuint [unitNums];
    memset(bitUnits,0,sizeof(myuint)*unitNums);
}
BitSequence::~BitSequence() {
    delete [] bitUnits;
}
bool BitSequence::test0(int position,int sequenceLength) {
    if(position>=sequenceLength) {
        cerr<<"[BitSequence::test0] Bit sequence error position"<<endl;
        exit(-1);
    }
    int unitIndex = position/32;
    int offset = position%32;
    return bitUnits[unitIndex].test0(offset);
}
bool BitSequence::test1(int position,int sequenceLength) {
    if(position>=sequenceLength) {
        cerr<<"[BitSequence::test0] Bit sequence error position"<<endl;
        exit(-1);
    }
    int unitIndex = position/32;
    int offset = position%32;
    return bitUnits[unitIndex].test1(offset);
}
void BitSequence::set0(int position,int sequenceLength) {
    if(position>=sequenceLength) {
        cerr<<"[BitSequence::set0] Bit sequence error position"<<endl;
        exit(-1);
    }
    int unitIndex = position/32;
    int offset = position%32;
    bitUnits[unitIndex].reset(offset);
}
void BitSequence::set1(int position,int sequenceLength) {
    if(position>=sequenceLength) {
        cerr<<"[BitSequence::set1] Bit sequence error position"<<endl;
        exit(-1);
    }
    int unitIndex = position/32;
    int offset = position%32;
    bitUnits[unitIndex].set(offset);
}

void BitSequence::duplicate(BitSequence *bitSequence,int sequenceLength) {
    int unitnum = ceil((float)sequenceLength/32.0);
    memcpy(this->bitUnits,bitSequence->bitUnits,sizeof(myuint)*unitnum);
}
/*****************************************************************/
/*RGNode::RGNode(int marking_length)
 * function: 构造函数，为marking数组申请空间，申请大小为哈希表大小，
 * 并全部初始化为0；
 * in: marking_length, 库所哈希表大小；
 * */
RGNode::RGNode() {
    marking = new Mark[MARKLEN];
//    memset(marking, 0, sizeof(Mark) * MARKLEN);
    next = NULL;
//    stubbornFlags = NULL;
//    fireableFlags = NULL;
}

RGNode::RGNode(RGNode *oldnode) {
    marking = new Mark[MARKLEN];
    for(int i=0;i<MARKLEN;i++) {
        marking[i] = oldnode->marking[i];
    }
//    memset(marking, 0, sizeof(Mark) * MARKLEN);
    next = NULL;
//    stubbornFlags = NULL;
//    fireableFlags = NULL;
}

/*int RGNode::Hash(int length)
 * function: 求当前marking的哈希值；哈希函数类似于BKDR，种子的值取3；
 * in: length, 状态数组长度
 * out: hash value;
 * */
index_t RGNode::Hash() {
    unsigned int seed = 3;
    unsigned int hash = 0;
    int i = 0;

    for (i; i < MARKLEN; i++) {
        hash = hash * seed + marking[i];
    }
    //int result = hash & 0x7fffffff;
    return hash;
}

bool RGNode::isFirable(const Transition &t) const {
    bool isfirable = true;
    vector<SArc>::const_iterator tpre = t.producer.begin();
    for (tpre; tpre != t.producer.end(); ++tpre) {
//        if(!petri->place[tpre->idx].significant) {
//            cerr<<"获取非重要库所token判断使能条件"<<endl;
//            continue;
//        }
        if (readPlace(tpre->idx) < tpre->weight) {
            isfirable = false;
            break;
        }
    }
    return isfirable;
}


/*RGNode::~RGNode()
 * function： 析构函数，释放空间
 * */
RGNode::~RGNode() {
    delete [] marking;
//    if(stubbornFlags)
//        delete stubbornFlags;
//    if(fireableFlags)
//        delete fireableFlags;
    MallocExtension::instance()->ReleaseFreeMemory();
}

void RGNode::printMarking(const int &len) {
    int i = 0;
    cout << "(";
    for (i; i < len - 1; ++i) {
        cout << marking[i] << ",";
    }
    cout << marking[i] << ")" << endl;
    /*print fireable transitions*/
//    set<index_t>::iterator it;
//    cout << "[";
//    for(it=fireset.begin();it!=fireset.end();++it) {
//        cout<<*it<<" ";
//    }
//    cout << "]"<<endl;
}

int RGNode::readPlace(int placeid) const {
    if(!SLICEPLACE)
        return marking[placeid];
    else if(petri->sliceExtra[placeid].significant) {
        int pos = petri->sliceExtra[placeid].project_idx;
        return marking[pos];
    }
    else {
//        cerr<<"[ERROR] read redundant place."<<endl;
        return -1;
    }
}

int RGNode::writePlace(int placeid, index_t tokencount) {
    if(!SLICEPLACE) {
        marking[placeid] = tokencount;
        return 0;
    }
    else if(petri->sliceExtra[placeid].significant) {
        marking[petri->sliceExtra[placeid].project_idx] = tokencount;
        return 0;
    }
    else {
//        cerr<<"非重要库所无法写入token值！"<<endl;
        return -1;
    }
}

bool RGNode::isFireable_by_flag(index_t tid) const {
    return isFirable(petri->transition[tid]);
}

//void RGNode::computeStubbornSet() {
//    stubbornFlags = new BitSequence(petri->transitioncount);
//    fireableFlags = new BitSequence(petri->transitioncount);
//
//    //compute reachable(t) of every visible t
////    set<index_t> stubbornSet;
//    if(ready2exit) {
//        return;
//    }
//    bool invisInclude = false;
//    queue<index_t> expandQueue;
//    for(index_t visT=0;visT<petri->transitioncount;visT++) {
//        if(petri->transition[visT].visible==false) {
//            continue;
//        }
//        //可视变迁必定属于顽固集合
//        stubbornFlags->set1(visT);
//        expandQueue.push(visT);
//        //expand
//        while (!expandQueue.empty()) {
//            index_t indexT = expandQueue.front();
//            expandQueue.pop();
//            Transition &expandingTransition = petri->transition[indexT];
//            //判断是否使能
//            bool fireable = isFirable(expandingTransition);
//            if(fireable) {
//                fireableFlags->set1(indexT);
//                for(int i=0;i<petri->transitioncount;i++) {
//                    if(!petri->accordWithMatrix[indexT][i]) {
//                        if(stubbornFlags->test0(i)) {
//                            stubbornFlags->set1(i);
//                            expandQueue.push(i);
//                            if(petri->transition[i].visible==false && isFirable(petri->transition[i]))
//                                invisInclude = true;
//                        }
//                    }
//                }
//            }
//            else {
//                set<index_t>::iterator increIter;
//                for(increIter=expandingTransition.increasingSet.begin();
//                    increIter!=expandingTransition.increasingSet.end();
//                    increIter++)
//                {
//                    if(stubbornFlags->test0(*increIter)) {
//                        stubbornFlags->set1(*increIter);
//                        expandQueue.push(*increIter);
//                        if(petri->transition[*increIter].visible==false && isFirable(petri->transition[*increIter]))
//                            invisInclude = true;
//                    }
//                }
//            }
//        }
//    }
//
//    if(!invisInclude) {
//        //invisable expand
//        for(int i=0;i<petri->transitioncount;i++) {
//            if(petri->transition[i].visible == false && isFirable(petri->transition[i])) {
//                //expand
//                expandQueue.push(i);
//                while(!expandQueue.empty()) {
//                    index_t indexT = expandQueue.front();
//                    expandQueue.pop();
//                    Transition &tt = petri->transition[indexT];
//                    bool fireable = isFirable(tt);
//                    if(fireable) {
//                        fireableFlags->set1(indexT);
//                        for(index_t accordTi=0;accordTi<petri->transitioncount;accordTi++) {
//                            if(stubbornFlags->test1(accordTi))
//                                continue;
//                            if(!petri->accordWithMatrix[indexT][accordTi]) {
//                                stubbornFlags->set1(accordTi);
//                                expandQueue.push(accordTi);
//                            }
//                        }
//                    }
//                    else {
//                        set<index_t>::iterator increIter;
//                        for(increIter=tt.increasingSet.begin();increIter!=tt.increasingSet.end();increIter++) {
//                            if(stubbornFlags->test1(*increIter))
//                                continue;
//                            stubbornFlags->set1(*increIter);
//                            expandQueue.push(*increIter);
//                        }
//                    }
//                }
//                break;
//            }
//        }
//    }
//}

//void RGNode::getFireSet(RGNode *lastnode, index_t lastfid) {
//    this->fireset = lastnode->fireset;
//    const Transition &t = petri->transition[lastfid];
//    set<index_t>::iterator deiter,eniter;
//    /*remove disabled transition*/
//    for(deiter=t.decreasing.begin();deiter!=t.decreasing.end();++deiter) {
//        set<index_t>::iterator it;
//        it = fireset.find(*deiter);
//        if(it!=fireset.end()) {
//            /*check if it is disabled*/
//            if(!isFirable(petri->transition[*deiter])) {
//                fireset.erase(it);
//            }
//        }
//    }
//    /*add enabled transition*/
//    for(eniter=t.increasing.begin();eniter!=t.increasing.end();++eniter) {
//        set<index_t>::iterator it;
//        it = fireset.find(*eniter);
//        if(it == fireset.end()) {
//            if(isFirable(petri->transition[*eniter])) {
//                fireset.insert(*eniter);
//            }
//        }
//    }
//}

/*****************************************************************/
/*RGNode::RGNode(int marking_length)
 * function: 构造函数，为marking数组申请空间，申请大小为哈希表大小，
 * 并全部初始化为0；
 * in: marking_length, 库所哈希表大小；
 * */
BitRGNode::BitRGNode() {
    marking = new myuint[FIELDCOUNT];
    memset(marking, 0, sizeof(myuint) * FIELDCOUNT);
    next = NULL;
//    fireableFlags = new BitSequence(transitioncount);
//    updateFlag = 0;
//    incre = decre = false;
//    stubbornFlags = NULL;
//    fireableFlags = NULL;
}


/*int RGNode::Hash(int length)
 * function: 求当前marking的哈希值；哈希函数类似于BKDR，种子的值取3；
 * in: length, 状态数组长度
 * out: hash value;
 * */
index_t BitRGNode::Hash() {
    unsigned int seed = 3;
    unsigned int hash = 0;
    int i = 0;
    unsigned int equvalue;
    for (i; i < FIELDCOUNT; i++) {
        memcpy(&equvalue, &marking[i], sizeof(unsigned int));
        hash = hash * seed + equvalue;
    }
    //int result = hash & 0x7fffffff;
    return hash;
}

bool BitRGNode::isFirable(const Transition &t) const {
    if(NUPN) {
        bool isfirable = true;
        vector<SArc>::const_iterator tpre = t.producer.begin();
        for (tpre; tpre != t.producer.end(); ++tpre) {
            if(!readPlace(tpre->idx)) {
                isfirable = false;
                break;
            }
        }
        return isfirable;
    }
    else if (SAFE) {
        bool isfirable = true;
        vector<SArc>::const_iterator tpre = t.producer.begin();
        for (tpre; tpre != t.producer.end(); ++tpre) {
            int unit = tpre->idx / (sizeof(myuint) * 8);
            int offset = tpre->idx % (sizeof(myuint) * 8);
            if (this->marking[unit].test0(offset)) {
                isfirable = false;
                break;
            }
        }
        return isfirable;
    }
    else if ((PINVAR && SLICEPLACE) || PINVAR || LONGBITPLACE) {
        bool isfirable = true;
        vector<SArc>::const_iterator tpre = t.producer.begin();
        for (tpre; tpre != t.producer.end(); ++tpre) {
            if(readPlace(tpre->idx)<tpre->weight) {
                isfirable = false;
                break;
            }
        }
        return isfirable;
    }
}

//bool BitRGNode::isFireable_by_flag(index_t tid) {
////    if(updated()) {
//        return fireableFlags->test1(tid,transitioncount);
////    }
////
////    return isFirable(petri->transition[tid]);
//}

/*RGNode::~RGNode()
 * function： 析构函数，释放空间
 * */
BitRGNode::~BitRGNode() {
    delete [] marking;
//    if(stubbornFlags)
//        delete stubbornFlags;
//    if(fireableFlags)
//        delete fireableFlags;
    MallocExtension::instance()->ReleaseFreeMemory();
}

void BitRGNode::printMarking(const int &len) {
    cout << "(";
    int i = 0;
    for (i; i < len - 1; ++i) {
        int unit = i / (sizeof(myuint) * 8);
        int offset = i % (sizeof(myuint) * 8);
        if (marking[unit].test0(offset))
            cout << "0,";
        else
            cout << "1,";
    }
    int unit = i / (sizeof(myuint) * 8);
    int offset = i % (sizeof(myuint) * 8);
    if (marking[unit].test0(offset))
        cout << "0)" << endl;
    else
        cout << "1)" << endl;
    /*print fireable transitions*/
//    set<index_t>::iterator it;
//    cout << "[";
//    for(it=fireset.begin();it!=fireset.end();++it) {
//        cout<<*it<<" ";
//    }
//    cout << "]"<<endl;
}

//void BitRGNode::computeStubbornSet() {
//    stubbornFlags = new BitSequence(petri->transitioncount);
//    fireableFlags = new BitSequence(petri->transitioncount);
//
//    //compute reachable(t) of every visible t
////    set<index_t> stubbornSet;
//    bool invisInclude = false;
//    bool *expanded = new bool[petri->transitioncount];
//    for(int i=0;i<petri->transitioncount;i++) {
//        expanded[i] = false;
//    }
//
//    queue<index_t> expandQueue;
//    for(index_t visT=0;visT<petri->transitioncount;visT++) {
//        if(ready2exit) {
//            delete [] expanded;
//            return;
//        }
//        if(petri->transition[visT].visible==false || expanded[visT]) {
//            continue;
//        }
//        //可视变迁必定属于顽固集合
//        stubbornFlags->set1(visT);
//        expandQueue.push(visT);
//        //expand
//        while (!expandQueue.empty()) {
//            index_t indexT = expandQueue.front();
//            expandQueue.pop();
//            if(expanded[indexT])
//                continue;
//            Transition &expandingTransition = petri->transition[indexT];
//            //判断是否使能
//            bool fireable = isFirable(expandingTransition);
//            if(fireable) {
//                fireableFlags->set1(indexT);
//                for(int i=0;i<petri->transitioncount;i++) {
//                    if(i!=indexT && !petri->accordWithMatrix[indexT][i]) {
//                        if(stubbornFlags->test0(i)) {
//                            stubbornFlags->set1(i);
//                            expandQueue.push(i);
//                            if(petri->transition[i].visible==false && isFirable(petri->transition[i]))
//                                invisInclude = true;
//                        }
//                    }
//                }
//            }
//            else {
//                set<index_t>::iterator increIter;
//                for(increIter=expandingTransition.increasingSet.begin();
//                    increIter!=expandingTransition.increasingSet.end();
//                    increIter++)
//                {
//                    if(stubbornFlags->test0(*increIter)) {
//                        stubbornFlags->set1(*increIter);
//                        expandQueue.push(*increIter);
//                        if(petri->transition[*increIter].visible==false && isFirable(petri->transition[*increIter]))
//                            invisInclude = true;
//                    }
//                }
//            }
//            expanded[indexT] = true;
//        }
//    }
//
//    if(!invisInclude) {
//        //invisable expand
//        for(int i=0;i<petri->transitioncount;i++) {
//            if(petri->transition[i].visible == false && isFirable(petri->transition[i])) {
//                //expand
//                expandQueue.push(i);
//                while(!expandQueue.empty()) {
//                    index_t indexT = expandQueue.front();
//                    expandQueue.pop();
//                    Transition &tt = petri->transition[indexT];
//                    bool fireable = isFirable(tt);
//                    if(fireable) {
//                        fireableFlags->set1(indexT);
//                        for(index_t accordTi=0;accordTi<petri->transitioncount;accordTi++) {
//                            if(stubbornFlags->test1(accordTi))
//                                continue;
//                            if(!petri->accordWithMatrix[indexT][accordTi]) {
//                                stubbornFlags->set1(accordTi);
//                                expandQueue.push(accordTi);
//                            }
//                        }
//                    }
//                    else {
//                        set<index_t>::iterator increIter;
//                        for(increIter=tt.increasingSet.begin();increIter!=tt.increasingSet.end();increIter++) {
//                            if(stubbornFlags->test1(*increIter))
//                                continue;
//                            stubbornFlags->set1(*increIter);
//                            expandQueue.push(*increIter);
//                        }
//                    }
//                }
//                break;
//            }
//        }
//    }
//    delete [] expanded;
//}

int BitRGNode::readPlace(int placeid) const{
    if(NUPN) {
        const NUPN_extra &pe = petri->nupnExtra[placeid];

        if(!pe.cutoff) {
            index_t markint;
            memcpy(&markint,&this->marking[pe.intnum],sizeof(index_t));
            if((markint & pe.low_read_mask)>>pe.intoffset == (pe.myoffset+1))
                return true;
            else
                return false;
        }
        else {
            index_t markint1,markint2;
            memcpy(&markint1,&this->marking[pe.intnum],sizeof(index_t));
            memcpy(&markint2,&this->marking[pe.intnum+1],sizeof(index_t));
            index_t read_content = ((markint1 & pe.low_read_mask)>>pe.intoffset) + ((markint2 & pe.high_read_mask)<<(32-pe.intoffset));
            return (read_content == pe.myoffset+1);
        }
    }
    else if(SAFE) {
        int unit = placeid / (sizeof(myuint) * 8);
        int offset = placeid % (sizeof(myuint) * 8);
        return this->marking[unit].test1(offset);
    }
    else if(PINVAR && SLICEPLACE) {
        if(!petri->sliceExtra[placeid].significant) {
            cerr<<"[ERROR@BitRGNode::readPlace] can not read places cut off by Petri-net slice "<<endl;
            return -1;
        }
        const Place_PINVAR_info &pinvarSliceInfo = petri->pinvarSliceExtra[placeid];

        if(pinvarSliceInfo.cutoff) {
            index_t markint1,markint2;
            memcpy(&markint1,&this->marking[pinvarSliceInfo.intnum],sizeof(index_t));
            memcpy(&markint2,&this->marking[pinvarSliceInfo.intnum+1],sizeof(index_t));
            markint1=markint1>>(pinvarSliceInfo.intoffset);
            int len1 = 32-pinvarSliceInfo.intoffset;
            int len2 = pinvarSliceInfo.intoffset+pinvarSliceInfo.length-32;
            markint2=markint2<<(32-len2);
            markint2=markint2>>(32-len2-len1);
            return (markint2+markint1);
        }
        else {
            index_t markint1;
            memcpy(&markint1,&this->marking[pinvarSliceInfo.intnum],sizeof(index_t));
            markint1 = markint1<<(32-pinvarSliceInfo.intoffset-pinvarSliceInfo.length);
            markint1 = markint1>>(32-pinvarSliceInfo.length);
            return markint1;
        }
    }
    else if(PINVAR) {
        const Place_PINVAR_info &pinvarInfo = petri->pinvarExtra[placeid];
        if(petri->pinvarExtra[placeid].significant) {
            //significant place, read directly
            if(pinvarInfo.cutoff) {
                index_t markint1,markint2;
                memcpy(&markint1,&this->marking[pinvarInfo.intnum],sizeof(index_t));
                memcpy(&markint2,&this->marking[pinvarInfo.intnum+1],sizeof(index_t));
                markint1=markint1>>(pinvarInfo.intoffset);
                int len1 = 32-pinvarInfo.intoffset;
                int len2 = pinvarInfo.intoffset+pinvarInfo.length-32;
                markint2=markint2<<(32-len2);
                markint2=markint2>>(32-len2-len1);
                return (markint2+markint1);
            } else {
                index_t markint1;
                memcpy(&markint1,&this->marking[pinvarInfo.intnum],sizeof(index_t));
                markint1 = markint1<<(32-pinvarInfo.intoffset-pinvarInfo.length);
                markint1 = markint1>>(32-pinvarInfo.length);
                return markint1;
            }
        }
        else {
            //redundant place, calculate its token by significant places
            double sum=0;
            for(int i=0;i<placecount;++i) {
                if(fabs(petri->Pinvar[pinvarInfo.pinvarLink][i])>1e-6 && petri->pinvarExtra[i].significant) {
                    sum += readPlace(i)*petri->Pinvar[pinvarInfo.pinvarLink][i];
                }
            }
            int ssum = sum<0?(int)(sum-0.5):(int)(sum+0.5);
            return (petri->weightsum0[pinvarInfo.pinvarLink]-ssum);
        }
    }
    else if(SLICEPLACE && LONGBITPLACE) {
        NUM_t markint;
        const P_SLICE_extra &PS_extra = petri->sliceExtra[placeid];
        if(PS_extra.significant)
            memcpy(&markint,&this->marking[PS_extra.project_idx],sizeof(index_t));
        else
            markint = MAXUINT32;
        return markint;
    }
    else if(LONGBITPLACE) {
        index_t markint;
        memcpy(&markint,&this->marking[placeid],sizeof(index_t));
        return markint;
    }
    cerr<<"ERROR, bit encoding for non-safe net."<<endl;
    exit(4);
}

void BitRGNode::writePlace(int placeid) {
    if(NUPN) {
        const NUPN_extra &pe = petri->nupnExtra[placeid];
        if(!pe.cutoff) {
            index_t markint;
            memcpy(&markint,&this->marking[pe.intnum],sizeof(index_t));
            markint = (markint & pe.low_zero_mask) | pe.low_write_mask;
            memcpy(&this->marking[pe.intnum],&markint,sizeof(index_t));
        }
        else {
            index_t markint1,markint2;
            memcpy(&markint1,&this->marking[pe.intnum],sizeof(index_t));
            memcpy(&markint2,&this->marking[pe.intnum+1],sizeof(index_t));
            markint1 = (markint1 & pe.low_zero_mask) | pe.low_write_mask;
            markint2 = (markint2 & pe.high_zero_mask) | pe.high_write_mask;
            memcpy(&this->marking[pe.intnum],&markint1,sizeof(index_t));
            memcpy(&this->marking[pe.intnum+1],&markint2,sizeof(index_t));
        }
    }
    else if(SAFE) {
        cerr<<"ERROR, function writePlace doesn't offer service for encoding of safe net."<<endl;
        exit(4);
    }
}

void BitRGNode::clearPlace(int placeid) {
    if(NUPN) {
        const NUPN_extra &pe = petri->nupnExtra[placeid];
        if(!pe.cutoff) {
            index_t markint;
            memcpy(&markint,&this->marking[pe.intnum],sizeof(index_t));
            markint = (markint & pe.low_zero_mask);
            memcpy(&this->marking[pe.intnum],&markint,sizeof(index_t));
        }
        else {
            index_t markint1,markint2;
            memcpy(&markint1,&this->marking[pe.intnum],sizeof(index_t));
            memcpy(&markint2,&this->marking[pe.intnum+1],sizeof(index_t));
            markint1 = (markint1 & pe.low_zero_mask);
            markint2 = (markint2 & pe.high_zero_mask);
            memcpy(&this->marking[pe.intnum],&markint1,sizeof(index_t));
            memcpy(&this->marking[pe.intnum+1],&markint2,sizeof(index_t));
        }
    }
    else if(SAFE) {
        cerr<<"ERROR, function clearPlace doesn't offer service for encoding of safe nets."<<endl;
        exit(4);
    }
}

int BitRGNode::writePlace(int placeid, index_t tokencount) {
    if(PINVAR && SLICEPLACE) {
        if(!petri->sliceExtra[placeid].significant) {
            cerr<<"[ERROR@BitRGNode::readPlace] can not write places cut off by Petri-net slice "<<endl;
            return -1;
        }
        const Place_PINVAR_info &pinvarSliceInfo = petri->pinvarSliceExtra[placeid];

        /*if(pinvarSliceInfo.length==1) {
            if(tokencount == 0) {
                this->marking[pinvarSliceInfo.intnum].reset(pinvarSliceInfo.intoffset);
            }
            else if(tokencount == 1) {
                this->marking[pinvarSliceInfo.intnum].set(pinvarSliceInfo.intoffset);
            }
        }*/

        if(pinvarSliceInfo.cutoff) {
            index_t zero_mask1,write_mask1,markint1;
            index_t zero_mask2,write_mask2,markint2;
            memcpy(&markint1,&this->marking[pinvarSliceInfo.intnum],sizeof(index_t));
            memcpy(&markint2,&this->marking[pinvarSliceInfo.intnum+1],sizeof(index_t));
            int len1,len2;
            len1 = 32-pinvarSliceInfo.intoffset;
            len2 = pinvarSliceInfo.intoffset + pinvarSliceInfo.length - 32;
            zero_mask1 = (0xffffffff<<len1)>>len1;
            zero_mask2 = (0xffffffff>>len2)<<len2;
            markint1 = markint1 & zero_mask1;
            markint2 = markint2 & zero_mask2;

            write_mask1 = tokencount<<pinvarSliceInfo.intoffset;
            write_mask2 = tokencount>>len1;
            if(tokencount>exp2(pinvarSliceInfo.length)-1) {
                cout<<tokencount;
                int num = exp2(pinvarSliceInfo.length)-1;
                cerr<<"ERROR, token-count over bound in P-invariant"<<endl;
                exit(4);
            }
            markint1 = markint1 | write_mask1;
            markint2 = markint2 | write_mask2;
            memcpy(&this->marking[pinvarSliceInfo.intnum],&markint1,sizeof(index_t));
            memcpy(&this->marking[pinvarSliceInfo.intnum+1],&markint2,sizeof(index_t));
        }
        else {
            index_t zero_mask,write_mask,markint;
            memcpy(&markint,&this->marking[pinvarSliceInfo.intnum],sizeof(index_t));
            zero_mask = 0xffffffff;
            zero_mask = zero_mask>>pinvarSliceInfo.intoffset;
            zero_mask = zero_mask<<(32-pinvarSliceInfo.length);
            zero_mask = zero_mask>>(32-pinvarSliceInfo.intoffset-pinvarSliceInfo.length);
            zero_mask = ~zero_mask;
            /*set corresponding place bits zero*/
            markint = markint & zero_mask;
            if(tokencount>exp2(pinvarSliceInfo.length)-1) {
                cout<<tokencount;
                int num = exp2(pinvarSliceInfo.length)-1;
                cerr<<"ERROR, token-count over bound in P-invariant"<<endl;
                exit(4);
            }
            write_mask = tokencount<<pinvarSliceInfo.intoffset;
            markint = markint | write_mask;
            memcpy(&this->marking[pinvarSliceInfo.intnum],&markint,sizeof(index_t));
        }
    }
    else if(PINVAR) {
        const Place_PINVAR_info &pinvarInfo = petri->pinvarExtra[placeid];
        if(pinvarInfo.significant) {

            /*if(pinvarInfo.length == 1) {
                if(tokencount == 0)
                    this->marking[pinvarInfo.intnum].reset(pinvarInfo.intoffset);
                else if(tokencount == 1)
                    this->marking[pinvarInfo.intnum].set(pinvarInfo.intoffset);
            }*/

            if(pinvarInfo.cutoff) {
                index_t zero_mask1,write_mask1,markint1;
                index_t zero_mask2,write_mask2,markint2;
                memcpy(&markint1,&this->marking[pinvarInfo.intnum],sizeof(index_t));
                memcpy(&markint2,&this->marking[pinvarInfo.intnum+1],sizeof(index_t));
                int len1,len2;
                len1 = 32-pinvarInfo.intoffset;
                len2 = pinvarInfo.intoffset + pinvarInfo.length - 32;
                zero_mask1 = (0xffffffff<<len1)>>len1;
                zero_mask2 = (0xffffffff>>len2)<<len2;
                markint1 = markint1 & zero_mask1;
                markint2 = markint2 & zero_mask2;

                write_mask1 = tokencount<<pinvarInfo.intoffset;
                write_mask2 = tokencount>>len1;
                if(tokencount>exp2(pinvarInfo.length)-1) {
                    cout<<tokencount;
                    int num = exp2(pinvarInfo.length)-1;
                    cerr<<"ERROR, token-count over bound in P-invariant"<<endl;
                    exit(4);
                }
                markint1 = markint1 | write_mask1;
                markint2 = markint2 | write_mask2;
                memcpy(&this->marking[pinvarInfo.intnum],&markint1,sizeof(index_t));
                memcpy(&this->marking[pinvarInfo.intnum+1],&markint2,sizeof(index_t));
            }
            else{
                index_t zero_mask,write_mask,markint;
                memcpy(&markint,&this->marking[pinvarInfo.intnum],sizeof(index_t));
                zero_mask = 0xffffffff;
                zero_mask = zero_mask>>pinvarInfo.intoffset;
                zero_mask = zero_mask<<(32-pinvarInfo.length);
                zero_mask = zero_mask>>(32-pinvarInfo.intoffset-pinvarInfo.length);
                zero_mask = ~zero_mask;
                /*set corresponding place bits zero*/
                markint = markint & zero_mask;
                if(tokencount>exp2(pinvarInfo.length)-1) {
                    cout<<tokencount;
                    int num = exp2(pinvarInfo.length)-1;
                    cerr<<"ERROR, token-count over bound in P-invariant"<<endl;
                    exit(4);
                }
                write_mask = tokencount<<pinvarInfo.intoffset;
                markint = markint | write_mask;
                memcpy(&this->marking[pinvarInfo.intnum],&markint,sizeof(index_t));
            }
        }
    }
    else if(SLICEPLACE && LONGBITPLACE) {
        const P_SLICE_extra &PS_extra = petri->sliceExtra[placeid];
        if(PS_extra.significant)
            memcpy(&this->marking[PS_extra.project_idx], &tokencount, sizeof(index_t));
    }
    else if(LONGBITPLACE) {
        memcpy(&this->marking[placeid],&tokencount,sizeof(index_t));
    }
    return 0;
}

//void BitRGNode::updateFlag_Incre() {
//    mtx.lock();
//    updateFlag++;
//    mtx.unlock();
//}
//
//bool BitRGNode::updated() {
//    bool updated = false;
//    mtx.lock();
//    updated = updateFlag >= 2 ? true : false;
//    mtx.unlock();
//    return updated;
//}

//void BitRGNode::handle_increasing_set(index_t lastid) {
//    Transition &tt = petri->transition[lastid];
//    //计算increasing set并判断是否使能
//    vector<SArc>::iterator postiter;
//    for(postiter=tt.consumer.begin();postiter!=tt.consumer.end();++postiter) {
//        if(petri->incidenceMatrix->getValue(lastid,postiter->idx)<=0)
//            continue;
//        Place &pp = petri->place[postiter->idx];
//        vector<SArc>::iterator iter;
//        for(iter=pp.consumer.begin();iter!=pp.consumer.end();iter++) {
//            if(fireableFlags->test1(iter->idx,transitioncount))
//                continue;
//            if(isFirable(petri->transition[iter->idx]))
//                fireableFlags->set1(iter->idx,transitioncount);
//        }
//    }
////    incre = true;
////    updateFlag_Incre();
////    cout<<"increasing set finished"<<endl;
//}
//
//void BitRGNode::handle_decreasing_set(index_t lastid) {
//    Transition &tt = petri->transition[lastid];
//    //计算decreasing set并判断是否使能
//    vector<SArc>::iterator preiter;
//    for(preiter=tt.producer.begin();preiter!=tt.producer.end();++preiter) {
//        if(petri->incidenceMatrix->getValue(lastid,preiter->idx)>=0)
//            continue;
//        Place &pp = petri->place[preiter->idx];
//        vector<SArc>::iterator iter;
//        for(iter=pp.consumer.begin();iter!=pp.consumer.end();iter++) {
//            if(fireableFlags->test0(iter->idx,transitioncount))
//                continue;
//            if(!isFirable(petri->transition[iter->idx]))
//                fireableFlags->set0(iter->idx,transitioncount);
//        }
//    }
////    decre = true;
////    updateFlag_Incre();
////    cout<<"decreasing set finished"<<endl;
//}
//
//void BitRGNode::get_fireset(BitRGNode *lastnode, index_t lastfid) {
//    fireableFlags->duplicate(lastnode->fireableFlags,transitioncount);
//    thread worker0 = thread(&BitRGNode::handle_increasing_set,this,lastfid);
//    thread worker1 = thread(&BitRGNode::handle_decreasing_set,this,lastfid);
//    worker0.join();
//    worker1.join();
//}

/****************************RG*****************************/
/*构造函数*/
RG::RG(Petri *pt, atomictable &argAT) : AT(argAT) {
    ptnet = pt;
    rgnode = new RGNode *[RGTABLE_SIZE];
    for (int i = 0; i < RGTABLE_SIZE; i++) {
        rgnode[i] = NULL;
    }
    initnode = NULL;
    hash_conflict_times = 0;
    nodecount = 0;

    RGNodelength = ptnet->placecount;

    int transcount = ptnet->transitioncount;
}

index_t RG::getHashIndex(RGNode *mark) {
    //计算哈希值
    index_t hashvalue = mark->Hash();
//    index_t size = RGTABLE_SIZE - 1;
    hashvalue = hashvalue % RGTABLE_SIZE;
    return hashvalue;
}

/*void RG::push(RGNode *mark)
 * function: 根据mark的哈希值将mark放进rgnode哈希表中
 * in: mark，待加入的状态节点
 * out:
 * */
void RG::addRGNode(RGNode *mark) {
    //计算哈希值
    //printRGNode(mark);
    index_t hashvalue = getHashIndex(mark);

    //计算哈希冲突次数
    if (rgnode[hashvalue] != NULL)
        hash_conflict_times++;

    nodecount++;

    //加入rgnode哈希表中,头插法
    mark->next = rgnode[hashvalue];
    rgnode[hashvalue] = mark;

//    cout<<nodecount<<endl;
#ifdef DEBUG
    mark->printMarking(ptnet->placecount);
    printRGNode(mark);
#endif

}

//计算初始状态节点
RGNode *RG::RGinitialnode() {
    RGNode *rg = new RGNode;

    //计算状态
    for (int i = 0; i < placecount; i++) {
        rg->writePlace(i,ptnet->place[i].initialMarking);
    }

    //将当前状态加入到rgnode哈希表中
    initnode = rg;
//    getFireableTranx(initnode,initnode->fireset);
    addRGNode(rg);
    return rg;
}

//当前状态cuenode发生transition[tranxnum]变前后所得到的状态，并把该状态返回
//若有数据类型溢出，则返回NULL
RGNode *RG::RGcreatenode(RGNode *curnode, int tranxnum, bool &exist) {

    RGNode *newnode = new RGNode(curnode);

    Transition *firingTanx = &ptnet->transition[tranxnum];
    vector<SArc>::iterator iterpre = firingTanx->producer.begin();
    vector<SArc>::iterator preend = firingTanx->producer.end();

    vector<SArc>::iterator iterpost = firingTanx->consumer.begin();
    vector<SArc>::iterator postend = firingTanx->consumer.end();

    //1.1 计算前继节点的token值；前继库所的token值=当前前继节点的token值-weight
    for (iterpre; iterpre != preend; iterpre++) {
//        if(!petri->place[iterpre->idx].significant)
//            continue;
        int ret = newnode->writePlace(iterpre->idx,newnode->readPlace(iterpre->idx)-iterpre->weight);
        if(ret == -1) {
            cerr<<"非重要库所无法写入token值！"<<endl;
            exit(5);
        }
    }

    //1.2 计算后继结点的token值；后继结点的token值=当前后继结点的token值+weight
    for (iterpost; iterpost != postend; iterpost++) {
        if(SLICEPLACE && !petri->sliceExtra[iterpost->idx].significant)
            continue;
        //判断数据类型溢出
        if ((int)iterpost->weight+(int)newnode->readPlace(iterpost->idx)<65535) {
            int ret = newnode->writePlace(iterpost->idx,newnode->readPlace(iterpost->idx)+iterpost->weight);
            if(ret == -1) {
                cerr<<"非重要库所无法写入token值！"<<endl;
                exit(5);
            }
        }
        else {
            delete newnode;
            return NULL;
        }
    }


    //3.判断是否已存在该节点
    index_t hashvalue = getHashIndex(newnode);
    bool repeated;
    RGNode *p = rgnode[hashvalue];

    //3.1遍历相同哈希值的状态节点，依次比较每一位
    while (p != NULL) {
        repeated = true;
        //比较每一位
        for (int i = 0; i < RGNodelength; i++) {
            if(SLICEPLACE && !petri->sliceExtra[i].significant)
                continue;
            if (newnode->readPlace(i) != p->readPlace(i)) {
                repeated = false;
                break;
            }
        }
        if (repeated) {
            exist = true;
            delete newnode;
            //MallocExtension::instance()->ReleaseFreeMemory();
            return p;
        }
        p = p->next;
    }

    //5.加入rgnode哈希表
    addRGNode(newnode);


//    extern ofstream debugout;
//    debugout << ptnet->transition[tranxnum].id << endl;

    //6.更新值atomic check avaliable  不管是哪种类型的LTL公式都需要更新前继和后继
//    if (false) {
//        //static ofstream debugout("checkpoint.txt",ios::app);
//        iterpre = firingTanx->producer.begin();
//        iterpost = firingTanx->consumer.begin();
//        vector<unsigned char>::iterator temp_it, temp_end;
//        // producer
//        for (; iterpre != preend; iterpre++) {
//            temp_it = ptnet->place[iterpre->idx].atomicLinks.begin();
//            temp_end = ptnet->place[iterpre->idx].atomicLinks.end();
//            for (; temp_it != temp_end; temp_it++) {
//                AT.atomics[*temp_it].last_check_avaliable = false;
//            }
//        }
//        // consumer
//        for (; iterpost != postend; iterpost++) {
//            temp_it = ptnet->place[iterpost->idx].atomicLinks.begin();
//            temp_end = ptnet->place[iterpost->idx].atomicLinks.end();
//            for (; temp_it != temp_end; temp_it++) {
//                AT.atomics[*temp_it].last_check_avaliable = false;
//            }
//        }
//    }

    return newnode;
}

RGNode *RG::RGcreatenode2(RGNode *curnode, int tranxnum, bool &exist) {

    RGNode *newnode = new RGNode(curnode);

    Transition *firingTanx = &ptnet->transition[tranxnum];
    vector<SArc>::iterator iterpre = firingTanx->producer.begin();
    vector<SArc>::iterator preend = firingTanx->producer.end();

    vector<SArc>::iterator iterpost = firingTanx->consumer.begin();
    vector<SArc>::iterator postend = firingTanx->consumer.end();

    //1.1 计算前继节点的token值；前继库所的token值=当前前继节点的token值-weight
    for (iterpre; iterpre != preend; iterpre++) {
        int ret = newnode->writePlace(iterpre->idx,newnode->readPlace(iterpre->idx)-iterpre->weight);
        if(ret == -1) {
            cerr<<"非重要库所无法写入token值！"<<endl;
            exit(5);
        }
    }

    //1.2 计算后继结点的token值；后继结点的token值=当前后继结点的token值+weight
    for (iterpost; iterpost != postend; iterpost++) {
        //判断数据类型溢出
        if(SLICEPLACE && !petri->sliceExtra[iterpost->idx].significant)
            continue;
        //判断数据类型溢出
        if ((int)iterpost->weight+(int)newnode->readPlace(iterpost->idx)<65535) {
            int ret = newnode->writePlace(iterpost->idx,newnode->readPlace(iterpost->idx)+iterpost->weight);
            if(ret == -1) {
                cerr<<"非重要库所无法写入token值！"<<endl;
                exit(5);
            }
        }
        else {
            delete newnode;
            return NULL;
        }
    }


    //3.判断是否已存在该节点
    index_t hashvalue = getHashIndex(newnode);
    bool repeated;
    RGNode *p = rgnode[hashvalue];

    //3.1遍历相同哈希值的状态节点，依次比较每一位
    while (p != NULL) {
        repeated = true;
        //比较每一位
        for (int i = 0; i < RGNodelength; i++) {
            if(SLICEPLACE && !petri->sliceExtra[i].significant)
                continue;
            if (newnode->readPlace(i) != p->readPlace(i)) {
                repeated = false;
                break;
            }
        }
        if (repeated) {
            exist = true;
            delete newnode;
            //MallocExtension::instance()->ReleaseFreeMemory();
            return p;
        }
        p = p->next;
    }

    //5.加入rgnode哈希表
//    addRGNode(newnode);
    return newnode;
}

//一次性生成全部状态空间
void RG::Generate(RGNode *node) {
    for(int i=0;i<ptnet->transitioncount;++i) {
        if(node->isFirable(ptnet->transition[i])) {
            bool exist = false;
            RGNode *nextnode = RGcreatenode(node, i, exist);
            if (!exist) {
                Generate(nextnode);
            }
        }
    }
}

//void RG::printRGNode(RGNode *node) {
//    outRG << "(";
//
//    for (int i = 0; i < RGNodelength; i++) {
//        outRG << node->marking[i] << ",";
//        cout << node->marking[i] << " ";
//    }
//    cout << endl;
//    outRG << ")[>";
///*
//    index_t *isFirable;
//    NUM_t firecount = 0;
//    getFireableTranx(node,&isFirable,firecount);
//    int fireT;
//    for(fireT=0; fireT<firecount; fireT++)
//    {
//        int tidx = isFirable[fireT];
//        outRG<<ptnet->transition[tidx].id<<" ";
//    }
//    outRG<<endl;
//    if(firecount > 0)
//        delete [] isFirable;*/
//}

/*析构函数，释放空间*/
RG::~RG() {
    int i = 0;
    for (i; i < RGTABLE_SIZE; i++) {
        if (rgnode[i] != NULL) {
            RGNode *p = rgnode[i];
            RGNode *q;
            while (p) {
                q = p->next;
                delete p;
                p = q;
            }
        }
    }
    delete [] rgnode;

    MallocExtension::instance()->ReleaseFreeMemory();
}

/****************************BitRG*****************************/
/*构造函数*/
BitRG::BitRG(Petri *pt, atomictable &argAT) : AT(argAT) {
    ptnet = pt;
    rgnode = new BitRGNode *[RGTABLE_SIZE];
    for (int i = 0; i < RGTABLE_SIZE; i++) {
        rgnode[i] = NULL;
    }
    initnode = NULL;
    hash_conflict_times = 0;
    nodecount = 0;
    if (NUPN) {
        RGNodelength = 0;
        for (int i = 0; i < pt->unitcount; i++) {
            RGNodelength += pt->unittable[i].mark_length;
        }
    }
    else if (SAFE) {
        RGNodelength = pt->placecount;
    }
    else if (PINVAR && SLICEPLACE) {
        RGNodelength = 0;
        for (int i = 0; i < pt->placecount; i++) {
            if(pt->sliceExtra[i].significant)
                RGNodelength += pt->pinvarSliceExtra[i].length;
        }
    }
    else if (PINVAR) {
        RGNodelength = 0;
        for (int i = 0; i < pt->placecount; i++) {
            if(pt->pinvarExtra[i].significant)
                RGNodelength += pt->pinvarExtra[i].length;
        }
    } else if (LONGBITPLACE) {
        RGNodelength = 32*pt->placecount;
    }

    int transcount = ptnet->transitioncount;
}


index_t BitRG::getHashIndex(BitRGNode *mark) {
    //计算哈希值
    index_t hashvalue = mark->Hash();
//    index_t size = RGTABLE_SIZE - 1;
    hashvalue = hashvalue % RGTABLE_SIZE;
    return hashvalue;
}

/*void BitRG::addRGNode(BitRGNode *mark)
 * function: 根据mark的哈希值将mark放进rgnode哈希表中
 * in: mark，待加入的状态节点
 * out:
 * */
void BitRG::addRGNode(BitRGNode *mark) {
    //计算哈希值
    //printRGNode(mark);
    index_t hashvalue = getHashIndex(mark);

    //计算哈希冲突次数
    if (rgnode[hashvalue] != NULL)
        hash_conflict_times++;

    nodecount++;

    //加入rgnode哈希表中,头插法
    mark->next = rgnode[hashvalue];
    rgnode[hashvalue] = mark;
//    cout<<"NODE-COUNT: "<<nodecount<<endl;
#ifdef DEBUG
    mark->printMarking(ptnet->placecount);
    printRGNode(mark);
#endif
}

//void BitRG::getFireableTranx(BitRGNode *curnode,set<index_t> &fireset) {
//
//    //计算当前状态的可发生变迁；
//    bool firable;
//    NUM_t tlength = ptnet->transitioncount;
//    Transition *tranx;
//
//    //遍历每一个变迁
//    for (index_t j = 0; j < tlength; j++) {
//        //对于第j个变迁
//        tranx = &ptnet->transition[j];
//        firable = true;
//
//        //遍历第j个变迁的所有前继库所
//        vector<SArc>::iterator iterpre = tranx->producer.begin();
//        vector<SArc>::iterator preend = tranx->producer.end();
//
//        //遍历ptnet.transition[j]的所有前继库所，看其token值是否大于weight
//        for (iterpre; iterpre != preend; iterpre++) {
//            if (NUPN) {
//                if (!curnode->readPlace(iterpre->idx)) {
//                    firable = false;
//                    break;
//                }
//            } else if (SAFE) {
//                int unit = iterpre->idx / (sizeof(myuint) * 8);
//                int offset = iterpre->idx % (sizeof(myuint) * 8);
//                if (curnode->marking[unit].test0(offset)) {
//                    firable = false;
//                    break;
//                }
//            } else if(PINVAR || LONGBITPLACE) {
//                if(curnode->readPlace(iterpre->idx)<iterpre->weight) {
//                    firable = false;
//                    break;
//                }
//            }
//        }
//
//        //判断是否能发生，若能发生，加入当前状态的可发生数组中
//        if (firable)
//            fireset.insert(j);
//    }
//}

/*void RG::RGinitialnode()
 * function:计算初始状态；分为三步：1.计算（marking），2.计算初始状态的可发生变迁，
 * 3.将当前状态加入到rgnode哈希表中
 * in:
 * out:
 * */

BitRGNode *BitRG::RGinitialnode() {
    BitRGNode *rg = new BitRGNode;

    //计算状态
    if (NUPN) {
        for (index_t i = 0; i < placecount; i++) {
            if(ptnet->place[i].initialMarking>0) {
                rg->writePlace(i);
            }
        }
    }
    else if (SAFE) {
        for (int i = 0; i < placecount; i++) {
            int unit = i / (sizeof(myuint) * 8);
            int offset = i % (sizeof(myuint) * 8);
            if (ptnet->place[i].initialMarking == 1) {
                rg->marking[unit].set(offset);
            } else {
                rg->marking[unit].reset(offset);
            }
        }
    }
    else if(PINVAR && SLICEPLACE) {
        for (index_t i = 0; i < placecount; i++) {
            if(ptnet->sliceExtra[i].significant) {
                rg->writePlace(i,ptnet->place[i].initialMarking);
            }
        }
    }
    else if (PINVAR) {
        for (index_t i = 0; i < placecount; i++) {
            if(ptnet->pinvarExtra[i].significant) {
                rg->writePlace(i,ptnet->place[i].initialMarking);
            }
        }
    }
    else if (LONGBITPLACE) {
        for (index_t i = 0; i < placecount; i++) {
            rg->writePlace(i,ptnet->place[i].initialMarking);
        }
    }

    //将当前状态加入到rgnode哈希表中
    initnode = rg;
//    get_init_fireset();
//    getFireableTranx(initnode,initnode->fireset);
    addRGNode(rg);
    MallocExtension::instance()->ReleaseFreeMemory();
    return rg;
}

/*RGNode *RG::RGcreatenode(RGNode *curnode, int tranxnum, bool &exist)
 * function: 根据当前节点rg, 和可发生变迁，创建新的状态节点；
 * 分为5步：1.计算状态值，2.判断是否无界，3.判断是否已存在该节点，4.计算可发生变迁，5.加入rgnode哈希表中
 * in:curnode,当前节点； tranxnum,transition表中第tranxnum个变迁
 * out:新加入节点的地址
 * */
BitRGNode *BitRG::RGcreatenode(BitRGNode *curnode, int tranxnum, bool &exist) {

    BitRGNode *newnode = new BitRGNode;
    //printRGNode(curnode);
    Transition *firingTanx = &ptnet->transition[tranxnum];
    vector<SArc>::iterator iterpre = firingTanx->producer.begin();
    vector<SArc>::iterator preend = firingTanx->producer.end();

    vector<SArc>::iterator iterpost = firingTanx->consumer.begin();
    vector<SArc>::iterator postend = firingTanx->consumer.end();

    /*************************caculate new marking********************************/
    if (NUPN) {
        memcpy(newnode->marking, curnode->marking, sizeof(myuint) * FIELDCOUNT);
        for (iterpre; iterpre != preend; iterpre++) {
            newnode->clearPlace(iterpre->idx);
        }

        for (iterpost; iterpost != postend; iterpost++) {
            newnode->writePlace(iterpost->idx);
        }
    }
    else if (SAFE) {
        memcpy(newnode->marking, curnode->marking, sizeof(myuint) * FIELDCOUNT);

        int unit;
        int offset;
        for (iterpre; iterpre != preend; iterpre++) {
            unit = iterpre->idx / (sizeof(myuint) * 8);
            offset = iterpre->idx % (sizeof(myuint) * 8);
            newnode->marking[unit].reset(offset);
        }

        for (iterpost; iterpost != postend; iterpost++) {
            unit = iterpost->idx / (sizeof(myuint) * 8);
            offset = iterpost->idx % (sizeof(myuint) * 8);
            newnode->marking[unit].set(offset);
        }
    }
    else if(PINVAR && SLICEPLACE) {
        memcpy(newnode->marking,curnode->marking,sizeof(myuint)*FIELDCOUNT);
        for(iterpre;iterpre!=preend;++iterpre) {
            if(ptnet->sliceExtra[iterpre->idx].significant) {
                index_t remain = newnode->readPlace(iterpre->idx) - iterpre->weight;
                newnode->writePlace(iterpre->idx,remain);
            }
        }
        for(iterpost;iterpost!=postend;++iterpost) {
            if(ptnet->sliceExtra[iterpost->idx].significant) {
                index_t newtoken = newnode->readPlace(iterpost->idx) + iterpost->weight;
                newnode->writePlace(iterpost->idx,newtoken);
            }
        }
    }
    else if (PINVAR) {
        memcpy(newnode->marking,curnode->marking,sizeof(myuint)*FIELDCOUNT);
        for(iterpre;iterpre!=preend;++iterpre) {
            if(ptnet->pinvarExtra[iterpre->idx].significant) {
                index_t remain = newnode->readPlace(iterpre->idx) - iterpre->weight;
                newnode->writePlace(iterpre->idx,remain);
            }
        }
        for(iterpost;iterpost!=postend;++iterpost) {
            if(ptnet->pinvarExtra[iterpost->idx].significant) {
                index_t newtoken = newnode->readPlace(iterpost->idx) + iterpost->weight;
                newnode->writePlace(iterpost->idx,newtoken);
            }
        }
    }
    else if(LONGBITPLACE) {
        memcpy(newnode->marking,curnode->marking,sizeof(myuint)*FIELDCOUNT);
        for(iterpre;iterpre!=preend;++iterpre) {
            index_t remain = newnode->readPlace(iterpre->idx) - iterpre->weight;
            newnode->writePlace(iterpre->idx,remain);
        }
        for(iterpost;iterpost!=postend;++iterpost) {
            index_t newtoken = newnode->readPlace(iterpost->idx) + iterpost->weight;
            newnode->writePlace(iterpost->idx,newtoken);
        }
    }

    /*************************check if repeated********************************/
    ThreadArg threadArg;
    threadArg.newnode = newnode;
    check_repeated(&threadArg);
//    thread worker = thread(&BitRG::check_repeated,this,&threadArg);
    if(threadArg.exist) {
        delete newnode;
        return threadArg.oldnode;
    }
    else {
        addRGNode(newnode);
        return newnode;
    }

}

void BitRG::check_repeated(ThreadArg *threadArg) {
    BitRGNode *newnode = threadArg->newnode;
    threadArg->exist = false;
    if(NUPN) {
        index_t hashvalue = getHashIndex(newnode);
        bool repeated;
        BitRGNode *p = rgnode[hashvalue];
        while (p != NULL) {
            repeated = true;
            index_t i = 0;
            for (i; i < FIELDCOUNT; i++) {
                unsigned int equp;
                unsigned int equnewnode;
                memcpy(&equp, &p->marking[i], sizeof(unsigned int));
                memcpy(&equnewnode, &newnode->marking[i], sizeof(unsigned int));
                if (equp != equnewnode) {
                    repeated = false;
                    break;
                }
            }
            if (repeated) {
                threadArg->oldnode = p;
                threadArg->exist = true;
                return;
            }
            p = p->next;
        }
    }
    else if(SAFE) {
        index_t hashvalue = getHashIndex(newnode);
        bool repeated;
        BitRGNode *p = rgnode[hashvalue];
        while (p != NULL) {
            repeated = true;
            index_t i = 0;
            for (i; i < FIELDCOUNT; i++) {
                unsigned int equp;
                unsigned int equnewnode;
                memcpy(&equp, &p->marking[i], sizeof(unsigned int));
                memcpy(&equnewnode, &newnode->marking[i], sizeof(unsigned int));
                if (equp != equnewnode) {
                    repeated = false;
                    break;
                }
            }

            if (repeated) {
                threadArg->oldnode = p;
                threadArg->exist = true;
                return;
            }
            p = p->next;
        }
    }
    else if(PINVAR && SLICEPLACE) {
        index_t hashvalue = getHashIndex(newnode);
        bool repeated;
        BitRGNode *p = rgnode[hashvalue];
        while(p!=NULL) {
            repeated = true;
            index_t i=0;
            for(i=0;i<FIELDCOUNT;++i) {
                unsigned int equp;
                unsigned int equnewnode;
                memcpy(&equp, &p->marking[i], sizeof(unsigned int));
                memcpy(&equnewnode, &newnode->marking[i], sizeof(unsigned int));
                if(equp != equnewnode) {
                    repeated = false;
                    break;
                }
            }
            if(repeated) {
                threadArg->oldnode = p;
                threadArg->exist = true;
                return;
            }
            p=p->next;
        }
    }
    else if(PINVAR) {
        index_t hashvalue = getHashIndex(newnode);
        bool repeated;
        BitRGNode *p = rgnode[hashvalue];
        while(p!=NULL) {
            repeated = true;
            index_t i=0;
            for(i=0;i<FIELDCOUNT;++i) {
                unsigned int equp;
                unsigned int equnewnode;
                memcpy(&equp, &p->marking[i], sizeof(unsigned int));
                memcpy(&equnewnode, &newnode->marking[i], sizeof(unsigned int));
                if(equp != equnewnode) {
                    repeated = false;
                    break;
                }
            }
            if(repeated) {
                threadArg->oldnode = p;
                threadArg->exist = true;
                return;
            }
            p=p->next;
        }
    }
    else if(LONGBITPLACE) {
        index_t hashvalue = getHashIndex(newnode);
        bool repeated;
        BitRGNode *p = rgnode[hashvalue];
        while(p!=NULL) {
            repeated = true;
            index_t i=0;
            for(i=0;i<FIELDCOUNT;++i) {
                unsigned int equp;
                unsigned int equnewnode;
                memcpy(&equp, &p->marking[i], sizeof(unsigned int));
                memcpy(&equnewnode, &newnode->marking[i], sizeof(unsigned int));
                if(equp != equnewnode) {
                    repeated = false;
                    break;
                }
            }
            if(repeated) {
                threadArg->oldnode = p;
                threadArg->exist = true;
                return;
            }
            p=p->next;
        }
    }
}


BitRGNode *BitRG::RGcreatenode2(BitRGNode *curnode, int tranxnum, bool &exist) {

    BitRGNode *newnode = new BitRGNode;
    //printRGNode(curnode);
    Transition *firingTanx = &ptnet->transition[tranxnum];
    vector<SArc>::iterator iterpre = firingTanx->producer.begin();
    vector<SArc>::iterator preend = firingTanx->producer.end();

    vector<SArc>::iterator iterpost = firingTanx->consumer.begin();
    vector<SArc>::iterator postend = firingTanx->consumer.end();

    if (NUPN) {
        memcpy(newnode->marking, curnode->marking, sizeof(myuint) * FIELDCOUNT);
        for (iterpre; iterpre != preend; iterpre++) {
            newnode->clearPlace(iterpre->idx);
        }

        for (iterpost; iterpost != postend; iterpost++) {
            newnode->writePlace(iterpost->idx);
        }

        //判断是否已经存在该节点
        index_t hashvalue = getHashIndex(newnode);

        bool repeated;
        BitRGNode *p = rgnode[hashvalue];
        while (p != NULL) {
            repeated = true;
            index_t i = 0;
            for (i; i < FIELDCOUNT; i++) {
                unsigned int equp;
                unsigned int equnewnode;
                memcpy(&equp, &p->marking[i], sizeof(unsigned int));
                memcpy(&equnewnode, &newnode->marking[i], sizeof(unsigned int));
                if (equp != equnewnode) {
                    repeated = false;
                    break;
                }
            }

            if (repeated) {
                exist = true;
                delete newnode;
                return p;
            }
            p = p->next;
        }

        //没有重复
//        addRGNode(newnode);
    }
    else if (SAFE) {
        memcpy(newnode->marking, curnode->marking, sizeof(myuint) * FIELDCOUNT);

        int unit;
        int offset;
        for (iterpre; iterpre != preend; iterpre++) {
            unit = iterpre->idx / (sizeof(myuint) * 8);
            offset = iterpre->idx % (sizeof(myuint) * 8);
            newnode->marking[unit].reset(offset);
        }

        for (iterpost; iterpost != postend; iterpost++) {
            unit = iterpost->idx / (sizeof(myuint) * 8);
            offset = iterpost->idx % (sizeof(myuint) * 8);
            newnode->marking[unit].set(offset);
        }

        index_t hashvalue = getHashIndex(newnode);

        bool repeated;
        BitRGNode *p = rgnode[hashvalue];
        while (p != NULL) {
            repeated = true;
            index_t i = 0;
            for (i; i < FIELDCOUNT; i++) {
                unsigned int equp;
                unsigned int equnewnode;
                memcpy(&equp, &p->marking[i], sizeof(unsigned int));
                memcpy(&equnewnode, &newnode->marking[i], sizeof(unsigned int));
                if (equp != equnewnode) {
                    repeated = false;
                    break;
                }
            }

            if (repeated) {
                exist = true;
                delete newnode;
                return p;
            }
            p = p->next;
        }

        //没有重复
//        addRGNode(newnode);
    }
    else if (PINVAR && SLICEPLACE) {
        memcpy(newnode->marking,curnode->marking,sizeof(myuint)*FIELDCOUNT);
        for(iterpre;iterpre!=preend;++iterpre) {
            if(ptnet->sliceExtra[iterpre->idx].significant) {
                index_t remain = newnode->readPlace(iterpre->idx) - iterpre->weight;
                newnode->writePlace(iterpre->idx,remain);
            }
        }
        for(iterpost;iterpost!=postend;++iterpost) {
            if(ptnet->sliceExtra[iterpost->idx].significant) {
                index_t newtoken = newnode->readPlace(iterpost->idx) + iterpost->weight;
                newnode->writePlace(iterpost->idx,newtoken);
            }
        }

        index_t hashvalue = getHashIndex(newnode);

        bool repeated;
        BitRGNode *p = rgnode[hashvalue];
        while(p!=NULL) {
            repeated = true;
            index_t i=0;
            for(i=0;i<FIELDCOUNT;++i) {
                unsigned int equp;
                unsigned int equnewnode;
                memcpy(&equp, &p->marking[i], sizeof(unsigned int));
                memcpy(&equnewnode, &newnode->marking[i], sizeof(unsigned int));
                if(equp != equnewnode) {
                    repeated = false;
                    break;
                }
            }
            if(repeated) {
                exist = true;
                delete newnode;
                return p;
            }
            p=p->next;
        }
    }
    else if (PINVAR) {
        memcpy(newnode->marking,curnode->marking,sizeof(myuint)*FIELDCOUNT);
        for(iterpre;iterpre!=preend;++iterpre) {
            if(ptnet->pinvarExtra[iterpre->idx].significant) {
                index_t remain = newnode->readPlace(iterpre->idx) - iterpre->weight;
                newnode->writePlace(iterpre->idx,remain);
            }
        }
        for(iterpost;iterpost!=postend;++iterpost) {
            if(ptnet->pinvarExtra[iterpost->idx].significant) {
                index_t newtoken = newnode->readPlace(iterpost->idx) + iterpost->weight;
                newnode->writePlace(iterpost->idx,newtoken);
            }
        }

        index_t hashvalue = getHashIndex(newnode);

        bool repeated;
        BitRGNode *p = rgnode[hashvalue];
        while(p!=NULL) {
            repeated = true;
            index_t i=0;
            for(i=0;i<FIELDCOUNT;++i) {
                unsigned int equp;
                unsigned int equnewnode;
                memcpy(&equp, &p->marking[i], sizeof(unsigned int));
                memcpy(&equnewnode, &newnode->marking[i], sizeof(unsigned int));
                if(equp != equnewnode) {
                    repeated = false;
                    break;
                }
            }
            if(repeated) {
                exist = true;
                delete newnode;
                return p;
            }
            p=p->next;
        }
        //addRGNode(newnode);
    }
    else if(LONGBITPLACE) {
        memcpy(newnode->marking,curnode->marking,sizeof(myuint)*FIELDCOUNT);
        for(iterpre;iterpre!=preend;++iterpre) {
            index_t remain = newnode->readPlace(iterpre->idx) - iterpre->weight;
            newnode->writePlace(iterpre->idx,remain);
        }
        for(iterpost;iterpost!=postend;++iterpost) {
            index_t newtoken = newnode->readPlace(iterpost->idx) + iterpost->weight;
            newnode->writePlace(iterpost->idx,newtoken);
        }

        index_t hashvalue = getHashIndex(newnode);

        bool repeated;
        BitRGNode *p = rgnode[hashvalue];
        while(p!=NULL) {
            repeated = true;
            index_t i=0;
            for(i=0;i<FIELDCOUNT;++i) {
                unsigned int equp;
                unsigned int equnewnode;
                memcpy(&equp, &p->marking[i], sizeof(unsigned int));
                memcpy(&equnewnode, &newnode->marking[i], sizeof(unsigned int));
                if(equp != equnewnode) {
                    repeated = false;
                    break;
                }
            }
            if(repeated) {
                exist = true;
                delete newnode;
                return p;
            }
            p=p->next;
        }
        //addRGNode(newnode);
    }
    return newnode;
}

void BitRG::Generate(BitRGNode *node) {
    for(int i=0;i<ptnet->transitioncount;++i) {
        if(node->isFirable(petri->transition[i])) {
            bool exist = false;
            BitRGNode *nextnode = RGcreatenode(node,i,exist);
            if(!exist) {
                Generate(nextnode);
            }
        }
    }
}

//void BitRG::printRGNode(BitRGNode *node) {
//    outRG << "(";
//    if (NUPN) {
//
//    } else if (SAFE) {
//        int unit;
//        int offset;
//        for (int i = 0; i < RGNodelength; i++) {
//            unit = i / (sizeof(myuint) * 8);
//            offset = i % (sizeof(myuint) * 8);
//            outRG << (node->marking[unit].test1(offset)) << ",";
//        }
//        outRG << ")[>";
//    }
//
//    index_t *isFirable;
//    NUM_t firecount = 0;
//    getFireableTranx(node, &isFirable, firecount);
//    int fireT;
//    for (fireT = 0; fireT < firecount; fireT++) {
//        int tidx = isFirable[fireT];
//        outRG << ptnet->transition[tidx].id << " ";
//    }
//    outRG << endl;
//    if (firecount > 0)
//        delete[] isFirable;
//}

/*析构函数，释放空间*/
BitRG::~BitRG() {
    int i = 0;
    for (i; i < RGTABLE_SIZE; i++) {
        if (rgnode[i] != NULL) {
            BitRGNode *p = rgnode[i];
            BitRGNode *q;
            while (p) {
                q = p->next;
                delete p;
                p = q;
            }
        }
    }
    delete rgnode;
    MallocExtension::instance()->ReleaseFreeMemory();
}

//void BitRG::get_init_fireset_thread(int myrank) {
//    for(int i=myrank;i<transitioncount;i+=4) {
//        if(initnode->isFirable(petri->transition[i]))
//            initnode->fireableFlags->set1(i,transitioncount);
//        else
//            initnode->fireableFlags->set0(i,transitioncount);
//    }
//}
//
//void BitRG::get_init_fireset() {
//    thread threads[4];
//    for(int i=0;i<4;i++) {
//        threads[i] = thread(&BitRG::get_init_fireset_thread,this,i);
//    }
//    for(int i=0;i<4;i++) {
//        threads[i].join();
//    }
//}

