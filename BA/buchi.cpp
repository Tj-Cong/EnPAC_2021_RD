//
// Created by hecong on 2020/8/5.
//
#include "buchi.h"
int timestamp = 0;

/*********************************************************\
|               Generalized Buchi Automata                |
\*********************************************************/
/*if there exists a transition delta' from this state that
 * delta'<=delta, then delta won't be added into the linked list
 * or
 * delta<=delta', then delta' will be removed from the linked list
 * and delta will be added into the linked list;
 *
 * else if there doesn't exist a transition delta' such that
 * delta'<=delta or delta<=delta',
 * then delta will be added into the linked list;
 * */
void GBA_state::AddArc(GBA_delta *delta) {
    if(this->firstarc == NULL) {
        this->firstarc = delta;
        arcnum++;
        return;
    }

    GBA_delta *p, *father;
    p=this->firstarc;
    if(p->trans <= delta->trans && SUBSETEQ(p->uids,delta->uids)) {
        delete delta;
        return;
    }
    else if(delta->trans <= p->trans && SUBSETEQ(delta->uids,p->uids)) {
       this->firstarc = p->next;
       delete p;
       delta->next = this->firstarc;
       this->firstarc = delta;
       return;
    }

    father = p;
    p = p->next;
    while(p) {
        if(p->trans <= delta->trans && SUBSETEQ(p->uids,delta->uids)) {
            delete delta;
            return;
        }
        else if(delta->trans <= p->trans && SUBSETEQ(delta->uids,p->uids)) {
            father->next = p->next;
            delete p;
            delta->next = this->firstarc;
            this->firstarc = delta;
            return;
        }
        father = p;
        p = p->next;
    }

    delta->next = this->firstarc;
    this->firstarc = delta;
    arcnum++;
}

bool EQUIVALENT(const GBA_state &s1, const GBA_state &s2) {
    if(s1.arcnum != s2.arcnum)
        return false;

    GBA_delta *p1,*p2;
    for(p1=s1.firstarc;p1;p1=p1->next) {
        for(p2=s2.firstarc;p2;p2=p2->next) {
            if(p1->trans == p2->trans)
                break;
        }
        if(p2==NULL)
            return false;
    }
    return true;
}

bool GBA_delta::implies(const GBA_delta &d2) {
    return (this->destination == d2.destination
           && SUBSETEQ(d2.trans.predicates, this->trans.predicates)
           && SUBSETEQ(this->uids, d2.uids));
}

GBA_delta *GBA_state::NEXT_NON_REDUNDANT(GBA_delta *begin) {
    GBA_delta *p=begin,*q;
    while (p) {
        if(!p->readyremove)
            return p;
        else {
            q=p;
            p=p->next;
            delete q;
            this->arcnum--;
        }
    }
    return NULL;
}

General::General() {
    vex_num = 0;
}

void General::Get_ustack(STNode *n) {
    if(n->ntyp == U_OPER) {
        ustack.insert(n);
        Get_ustack(n->nleft);
        Get_ustack(n->nright);
    }
    else {
        if(n->nleft)
            Get_ustack(n->nleft);
        if(n->nright)
            Get_ustack(n->nright);
    }
}

void General::Build_GBA(const Syntax_Tree &syntaxTree) {
    /*Get ustack first*/
    Get_ustack(syntaxTree.root);
    /*build initial state*/
    STNode *firstnode = syntaxTree.root->nleft;
    if(firstnode->ntyp == CONJUNC) {
        vertics[0].subformulas.insert(firstnode->nleft);
        STNode *p=firstnode->nright;
        while (p->ntyp == CONJUNC) {
            vertics[0].subformulas.insert(p->nleft);
            p=p->nright;
        }
        vertics[0].subformulas.insert(p);
    } else {
        vertics[0].subformulas.insert(firstnode);
    }
    vertics[0].id = vex_num;
    vex_num++;
    Build_GBA();
    state_num = vex_num;
}

void General::Build_GBA() {
    for(int i=0;i<vex_num;++i) {
        GBA_state &state = vertics[i];
        if(!state.explored) {
            vector<VWAA_delta> trans;
            VWAA_delta temp;                    //a tt transition,(tt,true);
            trans.push_back(temp);

            /*if this is a 'true' state, then create a tt transition from itself to itself*/
            if(state.subformulas.size() == 0) {
                GBA_delta *gbaDelta = new GBA_delta;
                CreateTransition(*gbaDelta,temp);
                gbaDelta->destination = i;
                state.AddArc(gbaDelta);
                state.explored = true;
                continue;
            }

            set<const STNode *>::iterator subf;
            for(subf=state.subformulas.begin();subf!=state.subformulas.end();++subf)
            {
                trans = trans * (*subf)->transitions;
            }
            //trans = delta''
            vector<VWAA_delta>::const_iterator item;
            for(item=trans.begin();item!=trans.end();++item) {
                GBA_delta *gbaDelta = new GBA_delta;
                CreateTransition(*gbaDelta,*item);
                state.AddArc(gbaDelta);
            }
            state.explored = true;
            CreateSuccessorState(i);
        }
    }
}

/*Add a new state into state list. The 'new' state consists of subformulas from subs
 * Notice: Before adding into the list, it must be checked that if there exists a
 * same state in the list. If the 'new' state is repeated, it won't be added into the list.
 * */
int General::AddState(const set<const STNode *> &subs) {
    for(int i=0;i<vex_num;++i) {
        if(vertics[i].subformulas == subs) {
            return i;
        }
    }
    vertics[vex_num].subformulas = subs;
    vertics[vex_num].id = vex_num;
    return (vex_num++);
}

void General::CreateSuccessorState(int stateid) {
    GBA_delta *p = vertics[stateid].firstarc;
    while (p) {
        int dest = AddState(p->trans.destinations);
        p->destination = dest;
        p=p->next;
    }
}

void General::CreateTransition(GBA_delta &gbaDelta, const VWAA_delta &vwaaDelta) {
    gbaDelta.trans = vwaaDelta;
    set<const STNode *>::const_iterator uiter,titer;
    for(uiter=ustack.begin();uiter!=ustack.end();++uiter) {
        bool exist = false;
        for(titer=vwaaDelta.destinations.begin();titer!=vwaaDelta.destinations.end();++titer) {
            /*this U subformula exists in vwaaDelta.destination*/
            if(*uiter == *titer) {
                exist = true;
                break;
            }
        }
        if(!exist) {
            gbaDelta.uids.insert((*uiter)->Uid);
        }
        else {
            vector<VWAA_delta>::const_iterator udelta;
            for(udelta=(*uiter)->transitions.begin();udelta!=(*uiter)->transitions.end();++udelta) {
                if(SUBSETEQ(vwaaDelta.predicates,udelta->predicates)
                    && SUBSETEQ(vwaaDelta.destinations,udelta->destinations)
                    && !BELONGTO(*uiter,udelta->destinations))
                {
                    gbaDelta.uids.insert((*uiter)->Uid);
                    break;
                }
            }
        }
    }
}

void General::Simplify() {
    do {
        simplest = true;
        Simplify_transition();
        Simplify_state();
    }while (!simplest);
}

void General::Simplify_state() {
    for(int s1=0;s1<vex_num;++s1) {
        GBA_state &state1 = vertics[s1];
        if(state1.id == -1)
            continue;
        for(int s2=s1+1;s2<vex_num;++s2) {
            GBA_state &state2 = vertics[s2];
            if(state2.id == -1)
                continue;
            if(EQUIVALENT(state1,state2)) {
                Merge_states(s1,s2);
                simplest = false;
            }
        }
    }
}

void General::Simplify_transition() {
    for(int i=0;i<vex_num;++i) {
        if(vertics[i].id == -1)
            continue;
        GBA_state &state = vertics[i];
        GBA_delta *d1,*d2;

        /*detect redundant transitions*/
        for(d1=state.firstarc;d1;d1=d1->next) {
            if(d1->readyremove)
                continue;
            for(d2=state.firstarc;d2;d2=d2->next) {
                if(d2->readyremove)
                    continue;
                if(d1!=d2 && d1->implies(*d2)) {
                    d2->readyremove = true;
                    simplest = false;
                }
            }
        }

        /*delete redundant transitions*/
        GBA_delta *p, *father;
        if(state.firstarc->readyremove) {
            p = state.NEXT_NON_REDUNDANT(state.firstarc);
            state.firstarc = p;
        }
        father = state.firstarc;
        p = state.firstarc->next;
        while(p) {
            p = state.NEXT_NON_REDUNDANT(p);
            father->next = p;
            if(p) {
                father = p;
                p = p->next;
            }
        }
    }
}

void General::Merge_states(int s1, int s2) {
    /*erase one of the state*/
    vertics[s2].id = -1;
    state_num--;

    /*let all transitions to s2 be redirected to s1*/
//    GBA_state &state2 = vertics[s2];
//    set<int>::iterator sid;
//    for(sid=state2.producers.begin();sid!=state2.producers.end();++sid) {
    for(int i=0;i<vex_num;++i) {
        /*iterate over vertics[sid]'s transitions to find the transition to s2*/
        if(vertics[i].id == -1)
            continue;
        GBA_delta *p = vertics[i].firstarc;
        while (p) {
            if(p->destination == s2)
                p->destination = s1;
            p=p->next;
        }
    }
}

/*check content:
 * 1. if state_num equals to the exact number of states;
 * 2. every state's arcnum equals to the exact number of transitions it has;
 * 3. there doesn't exist a transition which is to a non-sense state (whose id is -1);
 * 4. every state's id equals to its location in the state list
 * 5. every state's explored should be true;
 * */
void General::self_check() {
    int exact_state_num = 0;
    for(int i=0;i<vex_num;++i) {
        GBA_state &state = vertics[i];
        if(state.id == -1)
            continue;
        if(state.id!=i) {
            cerr<<"GBA self check failed, error code: [4]"<<endl;
            exit(-3);
        }
        exact_state_num++;
        GBA_delta *p=state.firstarc;
        int exact_arcnum=0;
        while (p) {
            exact_arcnum++;
            if(vertics[p->destination].id == -1) {
                cerr<<"GBA self check failed, error code: [3]"<<endl;
                exit(-3);
            }
            p=p->next;
        }
        if(state.arcnum!=exact_arcnum) {
            cerr<<"GBA self check failed, error code: [2]"<<endl;
            exit(-3);
        }
        if(!state.explored) {
            cerr<<"GBA self check failed, error code: [5]"<<endl;
            exit(-3);
        }
    }
    if(exact_state_num!=state_num) {
        cerr<<"GBA self check failed, error code: [1]"<<endl;
        exit(-3);
    }
}

/*********************************************************\
|                     Buchi Automata                      |
\*********************************************************/
bool BA_delta::implies(const BA_delta &delta) {
    return (this->destination==delta.destination
            && SUBSETEQ(delta.predicates,this->predicates));
}

void BA_state::AddArc(BA_delta *newdelta) {
    if(this->firstarc == NULL) {
        this->firstarc = newdelta;
        arcnum++;
        return;
    }

    if(this->firstarc->implies(*newdelta))
        return;

    if(newdelta->implies(*(this->firstarc))) {
        BA_delta *q = this->firstarc;
        newdelta->next = q->next;
        this->firstarc = newdelta;
        delete q;
        return;
    }

    BA_delta *p=firstarc->next, *father=firstarc;
    while (p) {
        if(p->implies(*newdelta))
            return;
        if(newdelta->implies(*p)) {
            father->next = p->next;
            delete p;
            newdelta->next = this->firstarc;
            this->firstarc = newdelta;
            return;
        }

        father = p;
        p=p->next;
    }

    newdelta->next = this->firstarc;
    this->firstarc = newdelta;
    arcnum++;
}

BA_delta *BA_state::NEXTNONREDUNT(BA_delta *begin) {
    BA_delta *p=begin,*q;
    while (p) {
        if(!p->readyremove)
            return p;
        q=p->next;
        delete p;
        p=q;
        arcnum--;
    }
    return NULL;
}

bool EQUIVALENT(const BA_state &s1, const BA_state &s2) {
    if(s1.arcnum!=s2.arcnum || s1.accepted!=s2.accepted)
        return false;

    BA_delta *p1,*p2;
    for(p1=s1.firstarc;p1;p1=p1->next) {
        for(p2=s2.firstarc;p2;p2=p2->next) {
            if(p1->predicates==p2->predicates && p1->destination==p2->destination)
                break;
        }
        if(p2==NULL)
            return false;
    }
    return true;
}

Buchi::Buchi() {
    vex_num = 0;
}

void Buchi::Build_BA(const General &gba) {
    ustacksize = gba.ustack.size();
    /*build initial state first, initial state is always stored in vertics[0]*/
    if(gba.vertics[0].id==-1) {
        cerr<<"Initial state error in GBA!"<<endl;
        exit(-4);
    }
    AddState(0,0);
    int stateid;
    while((stateid=NextVirginState())!=-1) {
        BA_state &baState = vertics[stateid];
        const GBA_state &gbaState = gba.vertics[baState.gba_stateid];
        GBA_delta *gbaDelta = gbaState.firstarc;
        int base;
        if(ustacksize == 0)
            base = 0;
        else
            base = baState.marknum % ustacksize;
        while (gbaDelta) {
            int mnum = base;
            for(int i=base+1;i<=ustacksize;++i) {
                if(BELONGTO(i,gbaDelta->uids))
                    mnum = i;
                else
                    break;
            }
            int dest = AddState(gbaDelta->destination,mnum);
            BA_delta *baDelta = new BA_delta(gbaDelta->trans.predicates,dest);
            baState.AddArc(baDelta);
            gbaDelta = gbaDelta->next;
        }
        baState.explored = true;
    }
    state_num=vex_num;
}

/*add a state (gid,num) into state list;
 * Notice: before adding, it must check if this state is a repeated one;
 * it it is, it won't be added into the list
 * */
int Buchi::AddState(int gid, int num) {
    /*checking if it is repeated*/
    for(int i=0;i<vex_num;++i) {
        if(vertics[i].gba_stateid==gid && vertics[i].marknum==num)
            return i;
    }

    BA_state &baState = vertics[vex_num];
    baState.id = vex_num;
    baState.gba_stateid = gid;
    baState.marknum = num;
    if(num == ustacksize) {
        baState.accepted = true;
    }
    return vex_num++;
}

int Buchi::NextVirginState() {
    for(int i=0;i<vex_num;++i) {
        if(!vertics[i].explored)
            return i;
    }
    return -1;
}

void Buchi::Simplify_state() {
    for(int s1=0;s1<vex_num;++s1) {
        if(vertics[s1].id == -1)
            continue;
        for(int s2=s1+1;s2<vex_num;++s2) {
            if(vertics[s2].id == -1)
                continue;
            if(EQUIVALENT(vertics[s1],vertics[s2])) {
                Merge_state(s1,s2);
                simplest = false;
            }
        }
    }
}

void Buchi::Simplify_transition() {
    for(int i=0;i<vex_num;++i) {
        if(vertics[i].id == -1)
            continue;
        BA_state &state = vertics[i];
        BA_delta *d1,*d2;

        /*detect redundant transitions*/
        for(d1=state.firstarc;d1;d1=d1->next) {
            if(d1->readyremove)
                continue;
            for(d2=state.firstarc;d2;d2=d2->next) {
                if(d2->readyremove)
                    continue;
                if(d1!=d2 && d1->implies(*d2)) {
                    d2->readyremove = true;
                    simplest = false;
                }
            }
        }

        /*delete redundant transitions*/
        BA_delta *p,*father;
        if(state.firstarc->readyremove) {
            state.firstarc = state.NEXTNONREDUNT(state.firstarc);
        }
        father = state.firstarc;
        p = state.firstarc->next;
        while (p) {
            p = state.NEXTNONREDUNT(p);
            father->next = p;
            if(p) {
                father = p;
                p = p->next;
            }
        }
    }
}

void Buchi::Simplify() {
    do {
        simplest = true;
        Simplify_transition();
        Simplify_state();
    } while(!simplest);
}

void Buchi::Merge_state(int s1, int s2) {
    /*erase one of the state*/
    vertics[s2].id = -1;
    state_num--;

    /*let all transitions to s2 be redirected to s1*/
    for(int i=0;i<vex_num;++i) {
        if(vertics[i].id == -1)
            continue;
        BA_delta *p = vertics[i].firstarc;
        while (p) {
            if(p->destination == s2)
                p->destination = s1;
            p=p->next;
        }
    }
}

void Buchi::PrintBuchi(string filename) {
    const char *file = filename.c_str();
    ofstream out(file, ios::out);
    out<<"digraph BA {"<<endl;
    for (int i = 0; i < vex_num; i++)
    {
        if(vertics[i].id == -1)
            continue;
        out<<"\t"<<"S"<<i<<" ";
        if(vertics[i].accepted) {
            out<<"[shape=doublecircle]"<<endl;
        } else{
            out<<"[shape=circle]"<<endl;
        }
    }
    out<<endl;
    for (int i = 0; i < vex_num; i++)
    {
        if(vertics[i].id == -1)
            continue;
        BA_delta *p = vertics[i].firstarc;
        while (p) {
            out<<"\t"<<"S"<<i<<"->S"<<p->destination<<" [label=\""<<p->info<<"\"]"<<endl;
            p = p->next;
        }
    }
    out<<"}"<<endl;
}

/*check content:
 * 1. if state_num equals to the exact number of states;
 * 2. every state's arcnum equals to the exact number of transitions it has;
 * 3. there doesn't exist a transition which is to a non-sense state (whose id is -1);
 * 4. every state's id equals to its location in the state list
 * 5. every state's explored should be true;
 * */
void Buchi::self_check() {
    int exact_state_num = 0;
    for(int i=0;i<vex_num;++i) {
        BA_state &state = vertics[i];
        if(state.id == -1)
            continue;
        if(state.id!=i) {
            cerr<<"BA self check failed, error code: [4]"<<endl;
            exit(-4);
        }
        exact_state_num++;
        BA_delta *p=state.firstarc;
        int exact_arcnum=0;
        while (p) {
            exact_arcnum++;
            if(vertics[p->destination].id == -1) {
                cerr<<"BA self check failed, error code: [3]"<<endl;
                exit(-4);
            }
            p=p->next;
        }
        if(state.arcnum!=exact_arcnum) {
            cerr<<"BA self check failed, error code: [2]"<<endl;
            exit(-4);
        }
        if(!state.explored) {
            cerr<<"BA self check failed, error code: [5]"<<endl;
            exit(-4);
        }
    }
    if(exact_state_num!=state_num) {
        cerr<<"BA self check failed, error code: [1]"<<endl;
        exit(-4);
    }
}

/*complete evey state's sources
 * */
void Buchi::Backward_chaining() {
    for(int i=0;i<vex_num;++i) {
        if(vertics[i].id == -1)
            continue;
        BA_delta *p = vertics[i].firstarc;
        while(p) {
            vertics[p->destination].sources.insert(i);
            p=p->next;
        }
    }
}

/*********************************************************\
|              State Based Buchi Automata                 |
\*********************************************************/
void Vertex::AddArc(int target) {
    consumers.insert(target);
}

/*Delete transitions p and transitions after p*/
void Vertex::DelArc(ArcNode *p) {
    ArcNode *q;
    while(p) {
        q=p->next;
        delete p;
        p=q;
    }
}

/*calculate the difficulty that satisfying the predicates of this state
 * difficulty = |AP|*/
void Vertex::CalDifficulty() {
    if(label=="true") {
        difficulty = 0;
        return;
    }
    difficulty = 1;
    string APs = label;
    int pos;
    while ((pos=APs.find_first_of("&&"))!=string::npos) {
        APs = APs.substr(pos+2);
        difficulty++;
    }
}

void StateBuchi::Build_SBA(const Buchi &ba) {
    for(int i=0;i<ba.vex_num;++i)
    {
        const BA_state &baState = ba.vertics[i];
        if(baState.id == -1)
            continue;
        set<int> ins,outs;
        set<int>::iterator basources;

        /*get all transitions which are to baState*/
        for(basources=baState.sources.begin();basources!=baState.sources.end();++basources)
        {
            BA_delta *p = ba.vertics[*basources].firstarc;
            while (p) {
                if(p->destination == i) {
                    if(p->sstateloc == -1) {  /*this ba transition doesn't have a corresponding sba state yet*/
                        /*create a new SBA state according to this BA_delta*/
                        Vertex &newsbastate = vertics[vex_num];
                        newsbastate.id = vex_num;
                        newsbastate.accepted = baState.accepted;
                        newsbastate.initial = false;
                        newsbastate.label = p->info;
                        ins.insert(newsbastate.id);
                        p->sstateloc = newsbastate.id;
                        vex_num++;
                    }
                    else {  /*this ba transition already have a corresponding sba state */
                        ins.insert(p->sstateloc);
                        vertics[p->sstateloc].accepted = baState.accepted;
                    }
                }
                p=p->next;
            }
        }

        /*get all transitions which are from baState*/
        BA_delta *q = baState.firstarc;
        while (q) {
            if(q->sstateloc == -1) {   /*this ba transition doesn't have a corresponding sba state yet*/
                Vertex &newsbastate = vertics[vex_num];
                newsbastate.id = vex_num;
                newsbastate.initial = (baState.id==0);
                newsbastate.accepted = ba.vertics[q->destination].accepted;
                newsbastate.label = q->info;
                outs.insert(newsbastate.id);
                q->sstateloc = newsbastate.id;
                vex_num++;
            }
            else {   /*this ba transition already have a corresponding sba state */
                outs.insert(q->sstateloc);
                vertics[q->sstateloc].initial = (baState.id == 0);
            }
            q = q->next;
        }

        /*create sbuchi transitions*/
        set<int>::iterator in,out;
        for(in=ins.begin();in!=ins.end();++in) {
            for(out=outs.begin();out!=outs.end();++out) {
                vertics[*in].AddArc(*out);
            }
        }
    }
    state_num = vex_num;
}

void StateBuchi::Simplify() {
    do {
        simplest = true;
        Simplify_state();
        PrintStateBuchi();
    } while (!simplest);
}

void StateBuchi::Simplify_state() {
    for(int i=0;i<vex_num;++i) {
        if(vertics[i].id == -1)
            continue;
        for(int j=i+1;j<vex_num;++j) {
            if(vertics[j].id == -1)
                continue;
            if(vertics[i].label==vertics[j].label
                && vertics[i].accepted==vertics[j].accepted
                && vertics[i].consumers==vertics[j].consumers)
            {
                Merge_state(i,j);
                simplest = false;
            }
        }
    }
}

void StateBuchi::Merge_state(int s1, int s2) {
    int removeid=s2,redirectid=s1;
    vertics[removeid].id = -1;
    if(vertics[removeid].initial)
        vertics[redirectid].initial = true;
    state_num--;

    /*redirect transitions*/
    Redirect_transitions(removeid,redirectid);
}

void StateBuchi::Redirect_transitions(int removeid, int redirectid) {
    for(int i=0;i<vex_num;++i) {
        if(vertics[i].id == -1)
            continue;
        set<int>::iterator iter = vertics[i].consumers.find(removeid);
        if(iter!=vertics[i].consumers.end()) {
            vertics[i].consumers.erase(iter);
            vertics[i].consumers.insert(redirectid);
        }
    }
}

/*complete producers information
 * Notice: called before function Add_heuristic()*/
void StateBuchi::Complete1() {
    for(int i=0;i<vex_num;++i) {
        Vertex &state = vertics[i];
        if(state.id == -1)
            continue;

        if(state.firstarc!=NULL) {
            cerr<<"Error in SBA complete function"<<endl;
            exit(-5);
        }

        set<int>::iterator iter;
        for(iter=state.consumers.begin();iter!=state.consumers.end();++iter)
        {
            vertics[*iter].producers.insert(i);
        }
    }
}

/*complete cost information and transition linked list
 * Notice: called after function Add_heuristic()*/
void StateBuchi::Complete2() {
    for(int i=0;i<vex_num;++i) {
        Vertex &state = vertics[i];
        if (state.id == -1)
            continue;

        state.CalDifficulty();
        state.cost = state.difficulty * 0.1 + state.mindistance;

        if (state.firstarc != NULL) {
            cerr << "Error in SBA complete function" << endl;
            exit(-5);
        }
    }
    
    for(int i=0;i<vex_num;++i) {
        Vertex &state = vertics[i];
        set<int>::iterator iter;
        for(iter=state.consumers.begin();iter!=state.consumers.end();++iter)
        {
            ArcNode *p;
            p = new ArcNode;
            p->destination = *iter;
            p->cost = vertics[p->destination].cost;
            if(state.firstarc == NULL
                || vertics[*iter].cost<vertics[state.firstarc->destination].cost)
            {
                p->next = state.firstarc;
                state.firstarc = p;
            }
            else {
                ArcNode *m = state.firstarc;
                ArcNode *n = m->next;
                while(n) {
                    if(vertics[*iter].cost>=vertics[m->destination].cost
                        && vertics[*iter].cost<vertics[n->destination].cost)
                    {
                        p->next = n;
                        m->next = p;
                        break;
                    }
                    m = n;
                    n = n->next;
                }
                if(n == NULL) {
                    m->next = p;
                }
            }
        }
    }
}

/*check content:
 * 1. if state_num equals to the exact number of states;
 * 2. there doesn't exist a transition which is to a non-sense state (whose id is -1);
 * 3. every state's id equals to its location in the state list
 * 4. consumers should be equal to transitions list
 * */
void StateBuchi::self_check() {
    int exact_state_num = 0;
    for(int i=0;i<vex_num;++i) {
        Vertex &state = vertics[i];
        if(state.id == -1) {
            state.DelArc(state.firstarc);
            state.consumers.clear();
            state.producers.clear();
            state.label="";
            state.firstarc = NULL;
            continue;
        }
        if(state.id!=i) {
            cerr<<"SBA self check failed, error code: [3]"<<endl;
            exit(-4);
        }
        exact_state_num++;
        ArcNode *p=state.firstarc;
        int exact_arcnum=0;
        while (p) {
            exact_arcnum++;
            if(vertics[p->destination].id == -1) {
                cerr<<"SBA self check failed, error code: [2]"<<endl;
                exit(-4);
            }

            if(state.consumers.find(p->destination)==state.consumers.end()) {
                cerr<<"SBA self check failed, error code: [4]"<<endl;
                exit(-4);
            }
            p=p->next;
        }
        if(state.consumers.size()!=exact_arcnum) {
            cerr<<"SBA self check failed, error code: [4]"<<endl;
            exit(-4);
        }
    }
    if(exact_state_num!=state_num) {
        cerr<<"SBA self check failed, error code: [1]"<<endl;
        exit(-4);
    }
}

void StateBuchi::Add_heuristic() {
    /*find accepted states*/
    vector<int> acceptedstates;
    for(int i=0;i<vex_num;++i) {
        if(vertics[i].id == -1)
            continue;
        if(vertics[i].accepted && vertics[i].trueaccpted) {
            acceptedstates.push_back(i);
            vertics[i].mindistance = 0;
        }
    }
    for(int j=0;j<acceptedstates.size();++j) {
        visited = new int[vex_num];
        memset(visited,0,sizeof(int)*vex_num);
        Backward(acceptedstates[j]);
        delete [] visited;
    }
}

void StateBuchi::Backward(int stateid) {
    Vertex &state = vertics[stateid];
    set<int>::iterator iter;
    for(iter=state.producers.begin();iter!=state.producers.end();++iter) {
        if(vertics[*iter].mindistance>state.mindistance+1) {
            vertics[*iter].mindistance = state.mindistance + 1;
        }
    }
    visited[stateid] = 1;
    for(iter=state.producers.begin();iter!=state.producers.end();++iter) {
        if(visited[*iter]==0) {
            Backward(*iter);
        }
    }
}

void StateBuchi::Tarjan(int stateid) {
    Vertex &state = vertics[stateid];
    if(state.id==-1) {
        cerr<<"Detect a non-sense state whose id is -1"<<endl;
        exit(-5);
    }

    state.dfn = state.low = timestamp++;
    tarjanstack.push(stateid);
    set<int>::iterator iter;
    for(iter=state.consumers.begin();iter!=state.consumers.end();++iter) {
        if(vertics[*iter].dfn == -1) {
            /*vertics[*iter] has not been visited*/
            Tarjan(*iter);
            state.low = (state.low<vertics[*iter].low)?state.low:vertics[*iter].low;
        }
        else if(tarjanstack.isinstack(*iter)) {
            state.low = (state.low<vertics[*iter].low)?state.low:vertics[*iter].low;
        }
    }
    if(state.dfn == state.low) {
        int popitem,poptimes=0;
        do {
            popitem = tarjanstack.pop();
            if(vertics[popitem].accepted && popitem!=stateid) {
                vertics[popitem].trueaccpted = true;
            }
            poptimes++;
        }while (popitem!=stateid);
        if(state.accepted && poptimes>=2) {
            state.trueaccpted = true;
        }
        else if(state.accepted && state.consumers.find(stateid)!=state.consumers.end()) {
            /*this accepted state is in a self cyclic*/
            state.trueaccpted = true;
        }
    }
}

void StateBuchi::Tarjan() {
    for(int i=0;i<vex_num;++i) {
        if(vertics[i].id!=-1 && vertics[i].initial) {
            tarjanstack.clear();
            Tarjan(i);
        }
    }
}

void StateBuchi::PrintStateBuchi() {
    ofstream outsba("SBA.dot",ios::out);
    if(!outsba) {
        cerr<<"CANNOT OPEN SBA.txt!"<<endl;
        exit(-1);
    }
    outsba<<"digraph SBA{"<<endl;
    outsba<<'\t'<<"rankdir=LR"<<endl;
    for(int i=0;i<vex_num;i++)
    {
        if(vertics[i].id == -1)
            continue;
        outsba<<"\t"<<i<<" ";
        if(vertics[i].accepted) {
            outsba<<"[shape=doublecircle,";
        } else{
            outsba<<"[shape=circle,";
        }
        if(vertics[i].initial) {
            outsba<<"style=filled,";
        }
        outsba<<"label=\""<<vertics[i].cost<<"\"]"<<endl;
    }
    outsba<<endl;
    for(int i=0;i<vex_num;i++) {
        if(vertics[i].id==-1)
            continue;
        set<int>::iterator iter;
        for(iter=vertics[i].consumers.begin();iter!=vertics[i].consumers.end();++iter) {
            outsba<<"\t"<<i<<"->"<<*iter<<endl;
        }
    }
    outsba<<"}"<<endl;
}

/*
void StateBuchi::linkAtomics(const atomictable &AT) {
    enum a_type{UNEXISTED,EXISTED_TRUE,EXISTED_FALSE};
    int i, j;
    string::size_type pos;
    atomic temp;
    a_type table[51];
    for (i = 0; i <= vex_num; i++) {
        if (vertics[i].id == -1 || vertics[i].label == "true")
            continue;//keep linklist empty
        //link

        for (j = 1; j <= AT.atomiccount; j++) {
            pos = 0;
            while ((pos = vertics[i].label.find(AT.atomics[j].mystr, pos)) != string::npos) {
                temp.atomicmeta_link = j;
                temp.negation = pos != 0 && vertics[i].label[pos - 1] == '!';
                vertics[i].links.push_back(temp);
                pos++;
            }
        }

        // invalid arg
        memset(&table,UNEXISTED,51*sizeof(a_type));//init table
        for (vector<atomic>::iterator it = vertics[i].links.begin(); it != vertics[i].links.end(); ++it) {
            if (table[(*it).atomicmeta_link] == EXISTED_FALSE && (vertics[i].invalid = (*it).negation))
                break;
            if (table[(*it).atomicmeta_link] == EXISTED_TRUE && (vertics[i].invalid = !(*it).negation))
                break;
            table[(*it).atomicmeta_link] = (*it).negation ? EXISTED_TRUE : EXISTED_FALSE;
        }


    }
}
*/


void StateBuchi::linkAtomics(atomictable &AT) {
    pAT = &AT;
    int i;
    for (i=0;i<=vex_num;i++){
        if (vertics[i].id == -1 || vertics[i].label == "true")
            continue;//keep linklist empty
        parseLabel(vertics[i].links, vertics[i].label, AT);

        // invalid arg
        judgeInvalid(vertics[i].invalid, vertics[i].links);
    }

    AT.linkPlace2atomictable();
}

void StateBuchi::parseLabel(vector<atomic> &links, const string &lable, const atomictable &AT) {
    string::size_type pos = 0, end;
    atomic temp;
    string substr;
    int i;
    while (true) {
        end = lable.find_first_of("&&", pos);
        if (lable[pos] == '!') {
            temp.negation = true;
            pos++;
        } else {
            temp.negation = false;
        }

        if (end == string::npos) {
            //last one
            substr = lable.substr(pos);
            for (i = 1; i <= AT.atomiccount; i++)
                if (AT.atomics[i].mystr == substr) {
                    temp.atomicmeta_link = i;
                    links.push_back(temp);
                    break;
                }
        } else {
            substr = lable.substr(pos, end - pos);
            for (i = 1; i <= AT.atomiccount; i++)
                if (AT.atomics[i].mystr == substr) {
                    temp.atomicmeta_link = i;
                    links.push_back(temp);
                    break;
                }
        }

        if (i > AT.atomiccount) {
            cerr << "Error in linking atomic string '" << substr << "'" << endl;
            exit(-1);
        }

        pos = end+2;
        if (end==string::npos)
            break;
    }
}

void StateBuchi::judgeInvalid(bool &invalid, const vector<atomic> &links) {
    enum a_type{UNEXISTED=0,EXISTED_TRUE,EXISTED_FALSE};
    a_type table[51];
    int i;
    memset(table,UNEXISTED,51*sizeof(a_type));
    for (auto link : links) {
        if (table[link.atomicmeta_link] == EXISTED_FALSE && (invalid = link.negation))
            return;
        if (table[link.atomicmeta_link] == EXISTED_TRUE && (invalid = !link.negation))
            return;
        table[link.atomicmeta_link] = link.negation ? EXISTED_TRUE : EXISTED_FALSE;
    }
    invalid= false;
}
