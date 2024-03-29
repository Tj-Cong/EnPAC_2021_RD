//
// Created by hecong on 2020/7/30.
//
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include "tinyxml.h"
#include "VWAA.h"
#include "atomic.h"
#include "Lexer.h"
#include <stack>
using namespace std;

/*ROOT: root node of Syntax_Tree, non-sense
 * PREDICATE: propositions;
 * NEG: negation;
 * CONJUNC: conjunction;
 * DISJUNC: disjunction;
 * NEXT: "X";
 * ALWAYS: "G";
 * EVENTUALLY: "F";
 * U_OPER: "U";
 * V_OPER: "V/R";
 * */
enum NodeType{ROOT,PREDICATE,NEG,CONJUNC,DISJUNC,NEXT,ALWAYS,EVENTUALLY,U_OPER,V_OPER};
enum privilege {unary=1,R_and_U=2,And=3,Or=4,Impli=5,Equ=6,root=7};
typedef struct stack_unit
{
    word w;
    privilege prilevel;
    stack_unit *next = NULL;
}LSNode;

class STACK {
private:
    stack<LSNode> elems;
public:
    void push(LSNode lsNode);
    bool pop(LSNode &lsNode);
    void pop();
    bool istoplpar();
    privilege topprilevel();
    bool empty();
};

/*The tree node structure of Syntax_Tree;
 * It also serves as a VWAA state
 * */
typedef struct STNode
{
    NodeType ntyp;      //node type
    int Uid;
    bool pure_even;     //whether the subformula is pure eventual, regarding the node as a root
    bool pure_univ;     //whether the subformula is pure universal, regarding the node as a root
    Evaluation groundtruth;
    string formula;     //the string of the subformula, regarding the node as a root
    set<index_t> relPlaceOrTransition;
    vector<set<const STNode *>> DNF;
    vector<VWAA_delta> transitions;    //the transition set of this node, regarding it as a VWAA state
    STNode *nleft;      //leftnode
    STNode *nright;     //rightnode
    STNode *parent;

    STNode() {
        groundtruth = UNKNOW;
        Uid = 0;
        pure_even = pure_univ = false;
        nleft = nright = NULL;
    }
    STNode(NodeType type) {
        ntyp = type;
        groundtruth = UNKNOW;
        Uid = 0;
        pure_even = pure_univ = false;
        nleft = nright = NULL;
    }
} STNode;

class Syntax_Tree
{
public:
    STNode *root;
    string propertyid;
    bool simplest;
    int UID;
    STACK Operand;
    STACK Operator;
    //
    atomictable AT;
    set<index_t> visibleIterms;
public:
    Syntax_Tree();
    ~Syntax_Tree();
    bool isBinaryOperator(NodeType ntype);
    bool isUnaryOperator(NodeType ntype);
    void reverse_polish(Lexer lex);
    void buildTreeFromCMD();
    /*Parse XML file which stores LTL formula;
     * filename: filename of XML file;
     * number: spefies which formula to parse, number~[1,16];
     * */
    int ParseXML(const char *filename, string &property, int number=1);

    /*Build a syntax tree from a XML structure; (recursive function)
     * xmlnode: xml node;
     * stnode: syntax tree node;
     * */
    void BuildTree(TiXmlElement *xmlnode, STNode* &stnode,bool &consistency);

    /*Tree operator*/
    void Destroy(STNode *n);
    void PrintTree() const;
    void PrintTree(STNode *n, int depth) const;

    /*Push negation into the formula and transform it to negation normal form;
     * (recursive function)*/
    void Push_Negation(STNode *n, STNode *nparent=NULL, bool right=0);

    /*Move negation down to predicates. (recursive function)*/
    void Negation_Down(STNode *n, STNode *nparent=NULL, bool right=0);

    /*Complete STNode information: formula, pure_even, pure_univ. (recursive function)*/
    void Perfect_Node(STNode *n);

    /*Simplify LTL formula*/
    void SimplifyLTL();
    void Rewrite(STNode *n, STNode *nparent=NULL, bool right=0);

    /*Replace "G","F" with V_OPER and U_OPER. (recursive fucntion)*/
    void Universe(STNode *n);

    void Evaluate(STNode *n);

    void Prune(STNode *curnode, STNode *predecessor);

    void getVisibleIterms();
    void getVisibleIterms(STNode *n);
    /*VWAA operation*/
    int AssignUID();
    void Get_DNF(STNode *n);
    void Build_VWAA();
    void Build_VWAA(STNode *n);
    void Transition_Simplify(STNode *n);
    void VWAA_Simplify();
    void VWAA_Simplify(STNode *n);

    bool isNextFree();
    bool isNextFree(STNode *n);
    /*atomicstable operation*/
    void PrintAT();
};