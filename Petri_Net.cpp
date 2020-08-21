//
// Created by lewis on 19-7-19.
//

#include<iostream>
#include"Petri_Net.h"
#include <signal.h>
#include <unistd.h>
#include <csetjmp>

using namespace std;

/***********Global Functions***********/


/*unsigned int BKDRHash(string str);
 * function: get a string's hash value
 * BKDR: HashValue(str) = sum(str[i]*seed^(length-i-1))
 * in: string str
 * out: unsigned int hash value
 * */
unsigned int BKDRHash(string str) {
    unsigned int seed = 31;
    unsigned int hash = 0;
    int length = str.length();
    int i = 0;
    for (i; i < length; i++) {
        hash = hash * seed + str[i];
    }
    hash = hash * seed;
    //保证最高位为0，这样在赋值给int型变量时，不用进行符号转换
    //hash = hash & 0x7fffffff;
    return hash;
}

void intersection(const vector<Small_Arc> &t1pre, const vector<Small_Arc> &t2pre, vector<int> &secidx) {
    secidx.clear();
    vector<Small_Arc>::const_iterator it1;
    vector<Small_Arc>::const_iterator it2;
    for (it1 = t1pre.begin(); it1 != t1pre.end(); ++it1) {
        for (it2 = t2pre.begin(); it2 != t2pre.end(); ++it2) {
            if (it1->idx == it2->idx)
                secidx.push_back(it1->idx);
        }
    }
}

/*int my_atoi(string str)
 * function: 将字符串转换为整型数据
 * in:string str, 由数字组成的字符串
 * out：int型数据
 * attention：该函数应当只能转换正整数，
 * 对于负数和小数，函数应当报错提示
 * */
int my_atoi(string str) {
    int len = str.length();
    int i;
    int result = 0;
    for (i = 0; i < len; i++) {
        if (str[i] >= '0' && str[i] <= '9') {
            result = result * 10 + (str[i] - '0');
        } else {
            cerr << "my_atoi error! The string contains illegal characters!" << endl;
            return -1;
        }
    }
    return result;
}

unsigned int stringToNum(const string &str) {
    istringstream iss(str);
    unsigned int num;
    iss >> num;
    return num;
}

/*****************Petri***************/
/*构造函数
 * */
Petri::Petri() {
    place = NULL;
    transition = NULL;
    arc = NULL;
    unittable = NULL;
    placecount = 0;
    transitioncount = 0;
    arccount = 0;
    NUPN = false;
    SAFE = false;
}

/*析构函数
 * */
Petri::~Petri() {
    delete[] place;
    delete[] transition;
    //delete[] arc;
    if (NUPN)
        delete[] unittable;
}

void Petri::getSize(char *filename) {
    //读取文档
    TiXmlDocument *mydoc = new TiXmlDocument(filename);
    if (!mydoc->LoadFile()) {
        cerr << mydoc->ErrorDesc() << endl;
    }

    //获得根元素
    TiXmlElement *root = mydoc->RootElement();
    if (root == NULL) {
        cerr << "Failed to load file: no root element!" << endl;
        mydoc->Clear();
    }
    string rootvalue = root->Value();

    TiXmlElement *net = root->FirstChildElement("net");
    TiXmlElement *page = net->FirstChildElement("page");

    //遍历每一页
    while (page) {
        TiXmlElement *pageElement = page->FirstChildElement();
        while (pageElement) {
            string value = pageElement->Value();
            if (value == "place") {
                placecount++;
            } else if (value == "transition") {
                transitioncount++;
            } else if (value == "arc") {
                arccount++;
            }
            else if (value == "toolspecific") {
                NUPN = true;
                SAFE = false;
                TiXmlElement *PTsize = pageElement->FirstChildElement("size");
                TiXmlAttribute *attr = PTsize->FirstAttribute();
                placecount = stringToNum(attr->Value());
                attr = attr->Next();
                transitioncount = stringToNum(attr->Value());
                attr = attr->Next();
                arccount = stringToNum(attr->Value());

                TiXmlElement *structure = pageElement->FirstChildElement("structure");
                TiXmlAttribute *usize = structure->FirstAttribute();
                unitcount = stringToNum(usize->Value());

                allocHashTable();    //申请空间

                preNUPN(structure);
                delete mydoc;
                return;
            }
            pageElement = pageElement->NextSiblingElement();
        }
        page = page->NextSiblingElement();
    }

    allocHashTable();
    delete mydoc;
}

/*void Petri::preNUPN(TiXmlElement *structure)
 * function: 读取NUPN的预处理；解析每一个单元，读取每一个单元中的
 * localplace, 然后按顺序存入place库所表中（该函数中没有该操作，
 * 只是将它在库所表中的索引位置存入Directory中）
 */
void Petri::preNUPN(TiXmlElement *structure) {
    TiXmlElement *unit = structure->FirstChildElement("unit");
    unsigned short ut_iter = 0;
    index_t pptr = 0;

    //读取每一个unit
    while (unit) {
        //获取unit的id
        TiXmlAttribute *uid = unit->FirstAttribute();
        unittable[ut_iter].uid = uid->Value();
        unittable[ut_iter].startpos = pptr;

        //get local places in this unit
        TiXmlElement *localplace = unit->FirstChildElement("places");
        //get local places string
        const char *test = localplace->GetText();
        if (test == NULL) {
            unit = unit->NextSiblingElement();
            unittable[ut_iter].size = 0;
            ut_iter++;
            continue;
        }
        string placestr = test;

        placestr = placestr+" ";
        string singleplace;
        NUM_t localplaces_count = 0;
        int startpos=0,endpos=0;
        char HT=9,space=32,CR=13;
        while(endpos<placestr.length()) {
            if(placestr[endpos]==HT || placestr[endpos]==space || placestr[endpos]==CR)
            {
                singleplace = placestr.substr(startpos,endpos-startpos);

                place[pptr].myunit = ut_iter;
                place[pptr].myoffset = localplaces_count;
                place[pptr].id = singleplace;
                mapPlace.insert(pair<string,index_t>(singleplace,pptr));

                localplaces_count++;
                ++pptr;
                startpos = ++endpos;
            } else {
                endpos++;
            }
        }
        unittable[ut_iter].size = localplaces_count;
        ut_iter++;
        unit = unit->NextSiblingElement();
    }

    if (ut_iter != unitcount)
        cerr << "Error! NUPN unitcount error!" << endl;
    if (pptr != placecount)
        cerr << "Error! NUPN placecount error!" << endl;
}

NUM_t Petri::getPlaceSize() const {
    return placecount;
}

/*void Petri::allocHashTable()
 * function: allocate space for place, transition and arc
 * */
void Petri::allocHashTable() {

    //allocate space for place, transition and arc
    place = new Place[placecount];
    transition = new Transition[transitioncount];
    arc = new Arc[arccount];

    if (NUPN) {
        unittable = new Unit[unitcount];
    }
}

/*int Petri::getPPosition(string str)
 * function: Given a string(a place's id), find its
 * index in place Hashtable. if couldn't find, return -1;
 * in:string str, place's id
 * out:place's index
 * */
index_t Petri::getPPosition(string str) {
    map<string,index_t>::iterator it;
    it=mapPlace.find(str);
    if(it==mapPlace.end()) {
        return INDEX_ERROR;
    }
    else {
        return it->second;
    }
}

/*int Petri::getTPosition(string str)
 * function: Given a string(a transition's id), find its
 * index in transition Hashtable. if couldn't find, return -1;
 * in:string str, transition's id
 * out:transition's index
 * */
index_t Petri::getTPosition(string str) {
    map<string,index_t>::iterator it;
    it=mapTransition.find(str);
    if(it==mapTransition.end()) {
        return INDEX_ERROR;
    }
    else {
        return it->second;
    }
}

index_t Petri::getPosition(string str, bool &isplace) {
    map<string,index_t>::iterator it;
    it=mapPlace.find(str);
    if(it!=mapPlace.end()) {
        isplace = true;
        return it->second;
    }

    it=mapTransition.find(str);
    if(it!=mapTransition.end()) {
        isplace = false;
        return it->second;
    }
    return INDEX_ERROR;
}

void Petri::readNUPN(char *filename) {
    TiXmlDocument *mydoc = new TiXmlDocument(filename);
    if (!mydoc->LoadFile()) {
        cerr << mydoc->ErrorDesc() << endl;
    }

    //获得根元素
    TiXmlElement *root = mydoc->RootElement();
    if (root == NULL) {
        cerr << "Failed to load file: no root element!" << endl;
        mydoc->Clear();
    }

    TiXmlElement *net = root->FirstChildElement("net");
    TiXmlElement *page = net->FirstChildElement("page");

    NUM_t tptr = 0;
    NUM_t aptr = 0;
    //遍历每一页
    while (page) {
        TiXmlElement *pageElement = page->FirstChildElement();
        while (pageElement) {
            string value = pageElement->Value();

            //库所
            if (value == "place") {
                Place p;
                TiXmlAttribute *id = pageElement->FirstAttribute();
                p.id = id->Value();
                TiXmlElement *initialmarking = pageElement->FirstChildElement("initialMarking");
                if (!initialmarking) {
                    p.initialMarking = 0;
                } else {
                    TiXmlElement *token = initialmarking->FirstChildElement("text");
                    if (token) {
                        string marking = token->GetText();
                        p.initialMarking = stringToNum(marking);
                    }
                }
                //
                index_t P_pidx = getPPosition(p.id);
                if (P_pidx == INDEX_ERROR) {
                    cerr << "getPPosition Error! Cannot find place!" << p.id << endl;
                    exit(0);
                }
                if(place[P_pidx].id != p.id) {
                    cerr << "mapPlace Error! place-id \""<<p.id<<"\" doesn't match!" << endl;
                    exit(0);
                }
                place[P_pidx].initialMarking = p.initialMarking;
            }
                //变迁
            else if (value == "transition") {
                //get information of Transition t;
                Transition &t = transition[tptr];
                TiXmlAttribute *id = pageElement->FirstAttribute();
                t.id = id->Value();

                mapTransition.insert(pair<string,index_t>(t.id,tptr));

                tptr++;

            }
                //弧
            else if (value == "arc") {
                Arc a;
                TiXmlAttribute *attr = pageElement->FirstAttribute();
                a.id = attr->Value();
                attr = attr->Next();
                a.source_id = attr->Value();
                attr = attr->Next();
                a.target_id = attr->Value();

                TiXmlElement *inscription = pageElement->FirstChildElement("inscription");
                if (!inscription) {
                    a.weight = 1;
                } else {
                    TiXmlElement *text = inscription->FirstChildElement("text");
                    if (text) {
                        string weight = text->GetText();
                        a.weight = stringToNum(weight);
                    }
                }
                arc[aptr++] = a;
            }
            pageElement = pageElement->NextSiblingElement();
        }
        page = page->NextSiblingElement();
    }

    //construct a petri net
    bool issourcePlace;
    int i = 0;
    int end = arccount;
    for (i; i < end; i++) {
        index_t index = getPosition(arc[i].source_id, issourcePlace);
        if (index != INDEX_ERROR) {
            arc[i].isp2t = issourcePlace;
            arc[i].source_idx = index;
            if (issourcePlace) {
                //该弧的前继节点是库所
                index_t tidx = getTPosition(arc[i].target_id);
                arc[i].target_idx = tidx;
                if (tidx != INDEX_ERROR) {
                    SArc place_post, tranx_pre;
                    place_post.weight = tranx_pre.weight = arc[i].weight;
                    place_post.idx = tidx;
                    tranx_pre.idx = index;
                    place[index].consumer.push_back(place_post);
                    transition[tidx].producer.push_back(tranx_pre);
                }
            } else {
                //该弧的前继节点是变迁
                index_t pidx = getPPosition(arc[i].target_id);
                arc[i].target_idx = pidx;
                if (pidx != INDEX_ERROR) {
                    SArc tranx_post, place_pre;
                    tranx_post.weight = place_pre.weight = arc[i].weight;
                    tranx_post.idx = pidx;
                    place_pre.idx = index;
                    place[pidx].producer.push_back(place_pre);
                    transition[index].consumer.push_back(tranx_post);
                }
            }
        }
    }

    computeUnitMarkLen();
    delete mydoc;
}

/*void Petri::readPNML(char *filename)
 * fucntion: Parse a PNML file, get its all
 * places, transitions and arcs, store them
 * in corresponding hashtables. After getting
 * them, construct a petri net(connect places
 * and arcs according to arcs)
 * */
void Petri::readPNML(char *filename) {
    //parse PNML
    //读取文档
    TiXmlDocument *mydoc = new TiXmlDocument(filename);
    if (!mydoc->LoadFile()) {
        cerr << mydoc->ErrorDesc() << endl;
    }

    //获得根元素
    TiXmlElement *root = mydoc->RootElement();
    if (root == NULL) {
        cerr << "Failed to load file: no root element!" << endl;
        mydoc->Clear();
    }

    TiXmlElement *net = root->FirstChildElement("net");
    TiXmlElement *page = net->FirstChildElement("page");

    index_t pptr = 0;
    index_t tptr = 0;
    index_t aptr = 0;
    //遍历每一页
    while (page) {
        TiXmlElement *pageElement = page->FirstChildElement();
        while (pageElement) {
            string value = pageElement->Value();

            //库所
            if (value == "place") {
                //获取库所信息
                Place &p = place[pptr];
                TiXmlAttribute *id = pageElement->FirstAttribute();
                p.id = id->Value();
                TiXmlElement *initialmarking = pageElement->FirstChildElement("initialMarking");
                if (!initialmarking) {
                    p.initialMarking = 0;
                } else {
                    TiXmlElement *token = initialmarking->FirstChildElement("text");
                    if (token) {
                        string marking = token->GetText();
                        unsigned int initm = stringToNum(marking);
                        if (initm > 65535) {
                            cout << "CANNOT_COMPUTE" << endl;
                            exit(0);
                        }
                        p.initialMarking = stringToNum(marking);
                    }
                }

                //将库所放入库所表中，并维护Directory
                mapPlace.insert(pair<string,index_t>(p.id,pptr));
                ++pptr;
            }
                //变迁
            else if (value == "transition") {
                //获取变迁信息
                Transition &t=transition[tptr];
                TiXmlAttribute *id = pageElement->FirstAttribute();
                t.id = id->Value();

                //将变迁放入变迁表中，并维护Directory
                mapTransition.insert(pair<string,index_t>(t.id,tptr));
                ++tptr;
            }
                //弧
            else if (value == "arc") {
                Arc a;
                TiXmlAttribute *attr = pageElement->FirstAttribute();
                a.id = attr->Value();
                attr = attr->Next();
                a.source_id = attr->Value();
                attr = attr->Next();
                a.target_id = attr->Value();

                TiXmlElement *inscription = pageElement->FirstChildElement("inscription");
                if (!inscription) {
                    a.weight = 1;
                } else {
                    TiXmlElement *text = inscription->FirstChildElement("text");
                    if (text) {
                        string weight = text->GetText();
                        a.weight = stringToNum(weight);
                    }
                }
                arc[aptr++] = a;
            }
            pageElement = pageElement->NextSiblingElement();
        }
        page = page->NextSiblingElement();
    }

    //construct a petri net
    bool issourcePlace;
    int i = 0;
    int end = arccount;
    for (i; i < end; i++) {
        int index = getPosition(arc[i].source_id, issourcePlace);
        if (index != INDEX_ERROR) {
            arc[i].isp2t = issourcePlace;
            arc[i].source_idx = index;
            if (issourcePlace) {
                //该弧的前继节点是库所
                int tidx = getTPosition(arc[i].target_id);
                arc[i].target_idx = tidx;
                if (tidx != INDEX_ERROR) {
                    SArc place_post, tranx_pre;
                    place_post.weight = tranx_pre.weight = arc[i].weight;
                    place_post.idx = tidx;
                    tranx_pre.idx = index;
                    place[index].consumer.push_back(place_post);
                    transition[tidx].producer.push_back(tranx_pre);
                }
            } else {
                //该弧的前继节点是变迁
                int pidx = getPPosition(arc[i].target_id);
                arc[i].target_idx = pidx;
                if (pidx != INDEX_ERROR) {
                    SArc tranx_post, place_pre;
                    tranx_post.weight = place_pre.weight = arc[i].weight;
                    tranx_post.idx = pidx;
                    place_pre.idx = index;
                    place[pidx].producer.push_back(place_pre);
                    transition[index].consumer.push_back(tranx_post);
                }
            }
        }
    }
    delete mydoc;
}

//计算每一个单元的marking长度
void Petri::computeUnitMarkLen() {
    index_t i = 0;
    index_t temp_len = 0;

    unittable[0].mark_sp = 0;
    unittable[0].mark_length = ceil(log2(unittable[i].size + 1));
    temp_len = unittable[0].mark_sp + unittable[0].mark_length;

    for (i = 1; i < unitcount; i++) {
        unittable[i].mark_sp = temp_len;
        unittable[i].mark_length = ceil(log2(unittable[i].size + 1));
        temp_len = temp_len + unittable[i].mark_length;
    }
}


//判断当前网是否为SAFE网
void Petri::judgeSAFE() {
    if (NUPN) {
        SAFE = false;
        return;
    }
    char filename[] = "GenericPropertiesVerdict.xml";

    for (int i = 0; i < arccount; ++i) {
        if (arc[i].weight > 1) {
            SAFE = false;
            return;
        }
    }

    ifstream infile(filename, ios::in);
    if (!infile) {
        SAFE = false;
        return;
    }

    TiXmlDocument *mydoc = new TiXmlDocument(filename);
    if (!mydoc->LoadFile()) {
        cerr << mydoc->ErrorDesc() << endl;
    }

    //获得根元素
    TiXmlElement *root = mydoc->RootElement();
    if (root == NULL) {
        cerr << "Failed to load file: no root element!" << endl;
        mydoc->Clear();
    }

    TiXmlElement *verdict = root->FirstChildElement("verdict");
    while (verdict) {
        TiXmlAttribute *attr = verdict->FirstAttribute();
        attr = attr->Next();
        string value = attr->Value();
        if (value == "SAFE") {
            attr = attr->Next();
            string isafe = attr->Value();
            if (isafe == "true") {
                SAFE = true;
            } else {
                SAFE = false;
            }
            return;
        }
        verdict = verdict->NextSiblingElement();
    }
}

void Petri::checkarc() {
    for (int i = 0; i < arccount; i++) {
        if (arc[i].isp2t)  //place->transition
        {
            if (place[arc[i].source_idx].id != arc[i].source_id) {
                cerr << "Error! Arc Check Failed @" << arc[i].source_id << endl;
                exit(-1);
            }

            if (transition[arc[i].target_idx].id != arc[i].target_id) {
                cerr << "Error! Arc Check Failed @" << arc[i].target_id << endl;
                exit(-1);
            }
        } else {
            if (transition[arc[i].source_idx].id != arc[i].source_id) {
                cerr << "Error! Arc Check Failed @" << arc[i].source_id << endl;
                exit(-1);
            }

            if (place[arc[i].target_idx].id != arc[i].target_id) {
                cerr << "Error! Arc Check Failed @" << arc[i].target_id << endl;
                exit(-1);
            }
        }
    }
    cout << "Arc Check Passed! ^_^" << endl;
    delete arc;
}

/*void printPlace();
 * function:按以下格式打印出所有的库所：
 * Total places：
 *
 * id：
 * index:
 * hashvalue:
 * initialMarking:
 * producer:
 * consumer:
id:p19
index:0
hashvalue:0
initialMarking0
producer:t21
consumer:t13

 * */
void Petri::printPlace() {
    ofstream outplace("places.txt", ios::out);
    outplace << "Total places：" << placecount << endl;

    int i;
    for (i = 0; i < placecount; i++) {
        Place p = place[i];
        outplace << endl;
        outplace << i << endl;
        outplace << "id:" << p.id << endl;
        outplace << "initialMarking:" << p.initialMarking << endl;
        outplace << "myUnit:" << p.myunit << endl;
        outplace << "myOffset:" << p.myoffset << endl;
        outplace << "producer:";
        vector<SArc>::iterator iterp;
        for (iterp = p.producer.begin(); iterp != p.producer.end(); iterp++) {
            outplace << transition[iterp->idx].id << " ";
        }
        outplace << endl;
        outplace << "consumer:";

        vector<SArc>::iterator iterc;
        for (iterc = p.consumer.begin(); iterc != p.consumer.end(); iterc++) {
            outplace << transition[iterc->idx].id << " ";
        }
        outplace << endl;
    }

}

/*void printTransition()
 * function:打印出所有的变迁信息
 * 打印格式：
 * Total Transitions：
 *
 * id:
 * index:
 * hashvalue:
 * producer:
 * consumer:
 * */
void Petri::printTransition() {
    ofstream outTransition("transition.txt", ios::out);
    outTransition << "Total transitions:" << transitioncount << endl;

    int i;
    for (i = 0; i < transitioncount; i++) {
        Transition &t = transition[i];
        outTransition << endl;
        outTransition << i << endl;
        outTransition << "id:" << t.id << endl;

        outTransition << "producer:";
        vector<SArc>::iterator iterp;
        for (iterp = t.producer.begin(); iterp != t.producer.end(); iterp++) {
            outTransition << "(" << iterp->weight << "," << place[iterp->idx].id << ") ";
        }
        outTransition << endl;
        outTransition << "consumer:";

        vector<SArc>::iterator iterc;
        for (iterc = t.consumer.begin(); iterc != t.consumer.end(); iterc++) {
            outTransition << "(" << iterc->weight << "," << place[iterc->idx].id << ") ";
        }
        outTransition << endl;
    }

}

void Petri::printGraph() {
    ofstream outGraph("PetriNetGraph.dot", ios::out);
    outGraph << "digraph PetriNet{" << endl;
    //先定义库所
    outGraph << '\t' << "rankdir=LR" << endl;
    index_t i = 0;
    for (i = 0; i < placecount; i++) {
        outGraph << '\t' << place[i].id << " [shape=circle]" << endl;
    }

    //再定义变迁
    for (i = 0; i < transitioncount; i++) {
        outGraph << '\t' << transition[i].id << " [shape=box]" << endl;
    }

    outGraph << endl;

    //打印边关系
    for (i = 0; i < arccount; i++) {
        outGraph << '\t' << arc[i].source_id << "->" << arc[i].target_id << " [label=\"" << arc[i].weight << "\"]"
                 << endl;
    }
    outGraph << "}" << endl;
}

void Petri::printUnit() {
    index_t i = 0;
    ofstream outUnit("unit.txt", ios::out);

    outUnit << "单元总数：" << unitcount << endl;
    outUnit << endl;
    for (i; i < unitcount; i++) {
        outUnit << "id: " << unittable[i].uid << endl;
        outUnit << "size: " << unittable[i].size << endl;
        outUnit << "Mark_Length: " << unittable[i].mark_length << endl;
        outUnit << "startpos: " << unittable[i].startpos << endl;
        outUnit << endl;
    }
}

void Petri::printTransition2CSV() {
    ofstream outTransition("transition.csv", ios::out);

    int i;
    for (i = 0; i < transitioncount; i++) {
        Transition &t = transition[i];
        outTransition << t.id << ",";

        vector<SArc>::iterator iterp;
        for (iterp = t.producer.begin(); iterp != t.producer.end(); iterp++) {
            outTransition <<place[iterp->idx].id << ",";
        }
        outTransition << "XXX,";
        vector<SArc>::iterator iterc;
        for (iterc = t.consumer.begin(); iterc != t.consumer.end(); iterc++) {
            outTransition << place[iterc->idx].id << ",";
        }
        outTransition << endl;
    }
}


