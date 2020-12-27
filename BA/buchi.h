//
// Created by hecong on 2020/8/5.
//
#include "Syntax_Tree.h"
using namespace std;

#define MAXSTATENUM 1000
#define MAXINT 0x7fffffff

class Buchi;
class StateBuchi;
/*********************************************************\
|               Generalized Buchi Automata                |
\*********************************************************/
typedef struct GBA_delta {
    int destination;             //targeted state's id
    VWAA_delta trans;
    set<int> uids;               //the accepted transition families it belongs to
    bool readyremove = false;    //this variable is used in  transition simplification
    GBA_delta *next = NULL;

    bool implies(const GBA_delta &d2);
} GBA_delta;

typedef struct GBA_state {
    int id = -1;                        //state id, also indicates its location in state list
    int arcnum = 0;                     //the number of arcs this state has
    bool explored = false;              //indicates if the transitions of this state have been calculated
//    set<int> producers;                 //states' id that have transitions to this state
    set<const STNode *> subformulas;    //subformulas this state consists of
    GBA_delta *firstarc = NULL;         //transitions from this state (linked list)

    void AddArc(GBA_delta *delta);      //add a transition to transition list
    GBA_delta * NEXT_NON_REDUNDANT(GBA_delta *begin);
    ~GBA_state() {
        GBA_delta *p=firstarc,*q;
        while(p) {
            q = p->next;
            delete p;
            p=q;
        }
    }
} GBA_state;

bool EQUIVALENT(const GBA_state &s1, const GBA_state &s2);

/*Generalized buchi automata
 * */
class General
{
private:
    int vex_num;
    int state_num;
    set<const STNode *> ustack;
    GBA_state vertics[MAXSTATENUM];
    bool simplest;
public:
    General();
    void Get_ustack(STNode *n);
    int AddState(const set<const STNode *> &subs);
    void CreateSuccessorState(int stateid);
    void CreateTransition(GBA_delta &gbaDelta, const VWAA_delta &vwaaDelta);
    void Build_GBA(const Syntax_Tree &syntaxTree);
    void Build_GBA();
    void Merge_states(int s1, int s2);
    void Simplify();
    void Simplify_state();
    void Simplify_transition();
    void self_check();
    friend class Buchi;
};

/*********************************************************\
|                     Buchi Automata                      |
\*********************************************************/
typedef struct BA_delta {
    bool readyremove = false;
    int sstateloc = -1;
    int destination;            //state's id (also its list location) which the transition is to
    set<string> predicates;     //conjunctions of propositions on this transitions
    string info;                //concatenate the propositions in 'predicates' by "&&" to form the string 'info'
    BA_delta *next = NULL;

    BA_delta(){};
    BA_delta(set<string> prop, int dest) {
        destination = dest;
        predicates = prop;
        if (predicates.size() == 0) {
            info = "true";
        }
        else {
            set<string>::iterator iter;
            for (iter = predicates.begin(); iter != predicates.end(); iter++) {
                info += (*iter) + "&&";
            }
            info = info.substr(0, info.length() - 2);
        }
    }
    bool implies(const BA_delta &delta);
} BA_delta;

/*a BA_state is composed of (gba_state,number)
 * if number = |ustack|, then it is accepted
 * */
typedef struct BA_state {
    int id = -1;                 //state id, also its location in state list; if id==-1, it means this state doesn't exist
    int gba_stateid;             //gba state id, also the gab state's location in gba state list
    int marknum;                 //number
    int arcnum = 0;              //the number of transitions which are from this state
    bool explored = false;       //if all the transitions from this state have been calculated
    bool accepted = false;       //if the state is an accepted state
    set<int> sources;            //states' id that have transitions to this state
    BA_delta *firstarc = NULL;

    void AddArc(BA_delta *delta);
    BA_delta *NEXTNONREDUNT(BA_delta *begin);
    ~BA_state() {
        BA_delta *p=firstarc, *q;
        while (p) {
            q=p->next;
            delete p;
            p=q;
        }
    }
} BA_state;

bool EQUIVALENT(const BA_state &s1, const BA_state &s2);

class Buchi
{
public:
    int vex_num;
    int state_num;
    int ustacksize;
    BA_state vertics[MAXSTATENUM];
    bool simplest;
public:
    Buchi();
    void Build_BA(const General &gba);
    int AddState(int gid, int num);
    int NextVirginState();
    void Merge_state(int s1, int s2);
    void Simplify();
    void Simplify_state();
    void Simplify_transition();
    void self_check();
    void Backward_chaining();
    void PrintBuchi(string filename);
    friend class StateBuchi;
};

/*********************************************************\
|              State Based Buchi Automata                 |
\*********************************************************/
typedef struct ArcNode {
    int destination;
    double cost;
    ArcNode *next = NULL;
} ArcNode;

typedef struct Vertex {
    int id = -1;                   /*state id, if it equals -1, it indicates this is a non-sense state*/
    bool initial = false;          /*indicate if this state is a initial state*/
    bool accepted = false;         /*indicate if this state is an accepted state*/
    bool trueaccpted = false;      /*indicate if this state (accepeted) could infinitely appear in a sequence*/
    string label;                  /*conjunction of propositions*/
    bool invalid = false;          /**/
    vector<atomic> links;          /**/
    double cost;                   /*the sum of heuristic information*/
    int mindistance = MAXINT;      /*minimum distance to a true accepted state*/
    int difficulty = 1;            /*the difficulty to satisfy this state's propositions*/
    int dfn = -1;                  /*timestamp used in tarjan*/
    int low = -1;                  /*the root of strongly connected component of this state*/
    set<int> producers;            /**/
    set<int> consumers;            /*successor states' id*/
    ArcNode *firstarc = NULL;

    ~Vertex() {
        ArcNode *p = firstarc,*q;
        while (p) {q=p->next;delete p;p=q;}
    }
    void CalDifficulty();
    void DelArc(ArcNode *p);
    void AddArc(int target);

    bool operator > (const Vertex &vex) const {
        return this->cost > vex.cost;
    }
    bool operator < (const Vertex &vex) const {
        return this->cost < vex.cost;
    }

} Vextex;

class StateBuchi
{
public:
    int vex_num;
    int state_num;
    Vertex vertics[MAXSTATENUM];
    bool simplest;
    CStack<int> tarjanstack;
    int *visited;
    atomictable *pAT;
public:
    StateBuchi(){vex_num=0;};
    void Build_SBA(const Buchi &ba);
    void Simplify();
    void Tarjan();
    void Add_heuristic();
    void Complete1();
    void Complete2();
    void self_check();
    void PrintStateBuchi();
    void linkAtomics(atomictable &AT);
private:
    void Simplify_state();
    void Merge_state(int s1,int s2);
    void Redirect_transitions(int removeid, int redirectid);
    void Tarjan(int stateid);
    void Backward(int stateid);
    static void parseLabel(vector<atomic> &links, const string &label, const atomictable &AT);
    static void judgeInvalid(bool &invalid, const vector<atomic> &links);
};