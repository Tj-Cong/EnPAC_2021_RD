#include "product.h"
#include "Petri_Net.h"
#include <iostream>
#include <sys/time.h>
using namespace std;

//ofstream debugout("checkpoint.txt");

#define TOTALTOOLTIME 3580

size_t  heap_malloc_total, heap_free_total,mmap_total, mmap_count;
void print_info() {
//    struct mallinfo mi = mallinfo();
//    printf("count by itself:\n");
//    printf("\033[31m\theap_malloc_total=%lu heap_free_total=%lu heap_in_use=%lu\n\tmmap_total=%lu mmap_count=%lu\n",
//           heap_malloc_total*1024, heap_free_total*1024, heap_malloc_total*1024-heap_free_total*1024,
//           mmap_total*1024, mmap_count);
//    printf("count by mallinfo:\n");
//    printf("\theap_malloc_total=%lu heap_free_total=%lu heap_in_use=%lu\n\tmmap_total=%lu mmap_count=%lu\n\033[0m",
//           mi.arena, mi.fordblks, mi.uordblks,
//           mi.hblkhd, mi.hblks);
    malloc_stats();
}

NUM_t FIELDCOUNT;
NUM_t placecount;
NUM_t MARKLEN;
bool NUPN = false;            //whether the checking process uses NUPN encoding
bool SAFE = false;            //whether the checking process uses SAFE encoding
bool PINVAR = false;          //whether the checking process uses P-invariant encoding
bool LONGBITPLACE = false;    //whether the checking process externs marking storage from short to int
bool ready2exit = false;
jmp_buf petrienv;

//以MB为单位
short int total_mem;          //memory limit
pid_t mypid;
Petri *petri = NULL;

double get_time() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec + t.tv_usec / 1000000.0;
}

void CHECKMEM() {
    mypid = getpid();
    total_mem = 16000;
}

void CONSTRUCTPETRI() {
    double starttime,endtime;
    Petri *ptnet = new Petri;
    char filename[]="model.pnml";
    ptnet->getSize(filename);
    if(ptnet->NUPN) {
        ptnet->readNUPN(filename);
    }
    else {
        ptnet->readPNML(filename);
    }
    ptnet->judgeSAFE();
    ptnet->judgePINVAR();
    setGlobalValue(ptnet);

//    starttime = get_time();
//    ptnet->computeDI();
//    endtime = get_time();
//    cout<<"COMPUTEDI TIME:"<<endtime-starttime<<endl;

    petri = ptnet;
//    if(NUPN)
//        ptnet->printUnit();
    ptnet->checkarc();
//    ptnet->printGraph();
//    ptnet->printPlace();
//    ptnet->printTransition();
//    ptnet->printTransition2CSV();
}
void CHECKLTL(Petri *ptnet, bool cardinality) {
    BitRG *bitgraph;
    RG *graph;
    double starttime,endtime;

    ofstream outresult("boolresult.txt", ios::app);  //outresult export results to boolresult.txt
    unsigned short ltlcount=0;
    unsigned short total_left_time=TOTALTOOLTIME;

    int i;
    string propertyid;
    char ff[]="LTLFireability.xml";
    char cc[]="LTLCardinality.xml";
    for(i=1;i<=16;++i)
    {
        unsigned short each_run_time;
        unsigned short each_used_time;
        if(ltlcount<6)
            each_run_time=300;
        else{
            each_run_time=total_left_time/(16-ltlcount);
        }
        Syntax_Tree syntaxTree;
        if(cardinality) {
            if(syntaxTree.ParseXML(cc,propertyid,i)==CONSISTENCY_ERROR) {
                cout << "FORMULA " << propertyid << " CANNOT_COMPUTE"<<endl;
                outresult << '?';
                continue;
            }
        }
        else {
            if(syntaxTree.ParseXML(ff,propertyid,i)==CONSISTENCY_ERROR) {
                cout << "FORMULA " << propertyid << " CANNOT_COMPUTE"<<endl;
                outresult << '?';
                continue;
            }
        }

        if(syntaxTree.root->groundtruth!=UNKNOW) {
            cout << "FORMULA " << propertyid << " " << ((syntaxTree.root->groundtruth==TRUE)?"TRUE":"FALSE")<<" TECHNIQUES SEQUENTIAL_PROCESSING"<<endl;
            outresult << ((syntaxTree.root->groundtruth==TRUE)?'T':'F');
            continue;
        }
//        cout<<"original tree:"<<endl;
//        syntaxTree.PrintTree();
//        cout << "-----------------------------------" << endl;
//        if (0) {
//            //debug output (Atomicstable)
//            cout << "***********************************" << endl;
//            syntaxTree.PrintAT();
//            cout << "***********************************" << endl;
//            continue;
//        }
        syntaxTree.Push_Negation(syntaxTree.root);
//        cout<<"after negation:"<<endl;
//        syntaxTree.PrintTree();
//        cout<<"-----------------------------------"<<endl;
        syntaxTree.SimplifyLTL();
//        cout<<"after simplification:"<<endl;
//        syntaxTree.PrintTree();
//        cout<<"-----------------------------------"<<endl;
        syntaxTree.Universe(syntaxTree.root);
//        cout<<"after universe"<<endl;
//        syntaxTree.PrintTree();
//        cout<<"-----------------------------------"<<endl;

        syntaxTree.Get_DNF(syntaxTree.root);
        syntaxTree.Build_VWAA();
        syntaxTree.VWAA_Simplify();

        General GBA;
        GBA.Build_GBA(syntaxTree);
        GBA.Simplify();
        GBA.self_check();

        Buchi BA;
        BA.Build_BA(GBA);
        BA.Simplify();
        BA.self_check();
        BA.Backward_chaining();
//        BA.PrintBuchi("BA.dot");

        StateBuchi SBA;
        SBA.Build_SBA(BA);
        SBA.Simplify();
        SBA.Tarjan();
        SBA.Complete1();
        SBA.Add_heuristic();
        SBA.Complete2();
        SBA.self_check();
//        SBA.PrintStateBuchi();
        SBA.linkAtomics(syntaxTree.AT);

        if (NUPN || SAFE || PINVAR || LONGBITPLACE) {
            bitgraph = new BitRG(ptnet, syntaxTree.AT);
//            BitRGNode *initnode = bitgraph->RGinitialnode();
//            bitgraph->Generate(initnode);
//            cout<<"STATE SPACE:"<<bitgraph->nodecount<<endl;
        } else {
            graph = new RG(ptnet, syntaxTree.AT);
//            RGNode *initnode = graph->RGinitialnode();
//            graph->Generate(initnode);
//            cout<<"STATE SPACE:"<<graph->nodecount<<endl;
        }

        //cout << "begin:ON-THE-FLY" << endl;
        ready2exit = false;
        if (NUPN || SAFE || PINVAR || LONGBITPLACE) {
            Product_Automata<BitRGNode, BitRG> *product;
            product = new Product_Automata<BitRGNode, BitRG>(ptnet, bitgraph, &SBA);

            starttime = get_time();
            each_used_time=product->ModelChecker(propertyid,each_run_time);
            endtime = get_time();
            cout<<endl;
            int ret = product->getresult();
            outresult << (ret == -1 ? '?' : (ret == 0 ? 'F' : 'T'));
            //cout << (ret == -1 ? '?' : (ret == 0 ? 'F' : 'T')) << endl;
            //cout<<"CONFLICT_TIMES:"<<product->getConflictTimes()<<endl;
            delete product;
        } else {
            Product_Automata<RGNode, RG> *product;
            product = new Product_Automata<RGNode, RG>(ptnet, graph, &SBA);

            starttime = get_time();
            each_used_time=product->ModelChecker(propertyid,each_run_time);
            endtime = get_time();
            cout<<endl;
            int ret = product->getresult();

            outresult << (ret == -1 ? '?' : (ret == 0 ? 'F' : 'T'));
            //cout<<"CONFLICT_TIMES:"<<product->getConflictTimes()<<endl;
            delete product;
        }
        if (NUPN || SAFE || PINVAR || LONGBITPLACE) {
            delete bitgraph;
        } else {
            delete graph;
        }
        total_left_time-=each_used_time;
        ltlcount++;
        MallocExtension::instance()->ReleaseFreeMemory();
    }
    outresult<<endl;
}

void CHECKLTL(Petri *ptnet,bool cardinality,int num) {
    BitRG *bitgraph;
    RG *graph;

    unsigned short each_run_time=900;

    string propertyid;
    char ff[]="LTLFireability.xml";
    char cc[]="LTLCardinality.xml";
    Syntax_Tree syntaxTree;
    if(cardinality) {
        if(syntaxTree.ParseXML(cc,propertyid,num)==CONSISTENCY_ERROR) {
            cout << "FORMULA " << propertyid << " CANNOT_COMPUTE CONSISTENCY_ERROR"<<endl;
            return;
        }
    }
    else {
        if(syntaxTree.ParseXML(ff,propertyid,num)==CONSISTENCY_ERROR) {
            cout << "FORMULA " << propertyid << " CANNOT_COMPUTE CONSISTENCY_ERROR"<<endl;
            return;
        }
    }

    cout<<"original tree:"<<endl;
    syntaxTree.PrintTree();
    if(syntaxTree.root->groundtruth!=UNKNOW) {
        cout << "FORMULA " << propertyid << " " << ((syntaxTree.root->groundtruth==TRUE)?"TRUE":"FALSE")<<endl;
    }
    cout<<"-----------------------------------"<<endl;
    if (0){
        //debug output (Atomicstable)
        cout<<"***********************************"<<endl;
        syntaxTree.PrintAT();
        cout<<"***********************************"<<endl;
        return ;
    }
    syntaxTree.Push_Negation(syntaxTree.root);
    cout<<"after negation:"<<endl;
    syntaxTree.PrintTree();
    cout<<"-----------------------------------"<<endl;
    syntaxTree.SimplifyLTL();
    cout<<"after simplification:"<<endl;
    syntaxTree.PrintTree();
    cout<<"-----------------------------------"<<endl;
    syntaxTree.Universe(syntaxTree.root);
    cout<<"after universe"<<endl;
    syntaxTree.PrintTree();
    cout<<"-----------------------------------"<<endl;



    syntaxTree.Get_DNF(syntaxTree.root);
    syntaxTree.Build_VWAA();
    syntaxTree.VWAA_Simplify();

    General GBA;
    GBA.Build_GBA(syntaxTree);
    GBA.Simplify();
    GBA.self_check();

    Buchi BA;
    BA.Build_BA(GBA);
    BA.Simplify();
    BA.self_check();
    BA.Backward_chaining();
    BA.PrintBuchi("BA.dot");

    StateBuchi SBA;
    SBA.Build_SBA(BA);
    SBA.PrintStateBuchi();
    SBA.Simplify();
    SBA.Tarjan();
    SBA.Complete1();
    SBA.Add_heuristic();
    SBA.Complete2();
    SBA.self_check();
    SBA.PrintStateBuchi();
    if (1){
        SBA.linkAtomics(syntaxTree.AT);
    }

    if (NUPN || SAFE || PINVAR || LONGBITPLACE) {
        bitgraph = new BitRG(ptnet, syntaxTree.AT);
//        BitRGNode *initnode = bitgraph->RGinitialnode();
//        bitgraph->Generate(initnode);
//        cout<<"STATE SPACE:"<<bitgraph->nodecount<<endl;
    } else {
        graph = new RG(ptnet, syntaxTree.AT);
//        RGNode *initnode = graph->RGinitialnode();
//        graph->Generate(initnode);
//        cout<<"STATE SPACE:"<<graph->nodecount<<endl;
    }


    ready2exit = false;
    if (NUPN || SAFE || PINVAR || LONGBITPLACE) {
        Product_Automata<BitRGNode, BitRG> *product;
        product = new Product_Automata<BitRGNode, BitRG>(ptnet, bitgraph, &SBA);
        product->ModelChecker(propertyid,each_run_time);
        cout<<endl;
        cout<<" "<<bitgraph->nodecount<<endl;
        int ret = product->getresult();
        //cout<<"CONFLICT_TIMES:"<<product->getConflictTimes()<<endl;
        delete product;
    } else {
        Product_Automata<RGNode, RG> *product;
        product = new Product_Automata<RGNode, RG>(ptnet, graph, &SBA);
        product->ModelChecker(propertyid,each_run_time);
        cout<<endl;
        cout<<" "<<graph->nodecount<<endl;
        int ret = product->getresult();

        //cout<<"CONFLICT_TIMES:"<<product->getConflictTimes()<<endl;
        delete product;
    }

    if (NUPN || SAFE || PINVAR || LONGBITPLACE) {
        delete bitgraph;
    } else {
        delete graph;
    }
}

int main0() {
    CHECKMEM();
    cout << "=================================================" << endl;
    cout << "=====This is our tool-EnPAC for the MCC'2021=====" << endl;
    cout << "=================================================" << endl;

    double starttime, endtime;
    starttime = get_time();

    CONSTRUCTPETRI();
    CHECKLTL(petri,1);
    CHECKLTL(petri,0);
    endtime = get_time();
    cout<<"RUNTIME:"<<endtime-starttime<<endl;
    delete petri;
    return 0;
}

int main(int argc,char *argv[])
{
    CHECKMEM();
    cout << "=================================================" << endl;
    cout << "=====This is our tool-EnPAC for the MCC'2021=====" << endl;
    cout << "=================================================" << endl;

    CONSTRUCTPETRI();

    string category = argv[1];
    if(category!="LTLFireability" && category!="LTLCardinality")
    {
        cout<<"DO_NOT_COMPETE"<<endl;
        exit(0);
    }

    if(category == "LTLCardinality") {
        CHECKLTL(petri,1);
    }

    if(category == "LTLFireability") {
        CHECKLTL(petri,0);
    }
}

//void CreateBA(Buchi &ba) {
//}