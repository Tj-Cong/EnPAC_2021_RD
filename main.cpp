#include "product.h"
#include "Petri_Net.h"
#include <iostream>
#include <sys/time.h>
using namespace std;

//ofstream debugout("checkpoint.txt");

#define TOTALTOOLTIME 3500

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
NUM_t transitioncount;
NUM_t MARKLEN;
bool NUPN = false;            //whether the checking process uses NUPN encoding
bool SAFE = false;            //whether the checking process uses SAFE encoding
bool PINVAR = false;          //whether the checking process uses P-invariant encoding
bool LONGBITPLACE = false;    //whether the checking process externs marking storage from short to int
bool SLICEPLACE = true;       //whether using Petri-nets slice (cut off places)
bool SLICETRANSITION = true;  //whether using Petri-nets slice (cut off transitions)
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
//    PNMLParser::getInstance()->parse(ptnet);
    ptnet->constructMatrix();
    ptnet->judgeSAFE();
    ptnet->judgePINVAR();
    ptnet->destroyMatrix();
//    ptnet->computeDI();
//    ptnet->computeAccordWith();
//    ptnet->printAccordWith();

    petri = ptnet;
    petri->setGlobalPetriAttribute();
//    if(NUPN)
//        ptnet->printUnit();
//    ptnet->printGraph();
//    ptnet->printPlace();
//    ptnet->printTransition();
//    ptnet->printTransition2CSV();
    ptnet->checkarc();
}
void CHECKLTL(Petri *ptnet, bool cardinality) {
    BitRG *bitgraph;RG *graph;
    double starttime,endtime;
    ofstream outresult("boolresult.txt", ios::app);  //outresult export results to boolresult.txt
    unsigned short total_left_time=TOTALTOOLTIME;

    string propertyid;
    char ff[]="LTLFireability.xml";
    char cc[]="LTLCardinality.xml";
    Syntax_Tree *syntaxTree;
    General *GBA;
    Buchi *BA;
    StateBuchi *SBA;
    for(int i=1;i<=16;++i)
    {
        unsigned short each_run_time;
        if(i<=6)
            each_run_time=300;
        else{
            each_run_time=total_left_time/(17-i);
        }

        /************************!!set timer!!*************************/
        starttime = get_time();
        syntaxTree = new Syntax_Tree;
        if(cardinality) {
            if(syntaxTree->ParseXML(cc,propertyid,i)==CONSISTENCY_ERROR) {
                cout << "FORMULA " << propertyid << " CANNOT_COMPUTE"<<endl;
                outresult << '?';
                continue;
            }
        }
        else {
            if(syntaxTree->ParseXML(ff,propertyid,i)==CONSISTENCY_ERROR) {
                cout << "FORMULA " << propertyid << " CANNOT_COMPUTE"<<endl;
                outresult << '?';
                continue;
            }
        }

        syntaxTree->Push_Negation(syntaxTree->root);
        syntaxTree->SimplifyLTL();
        syntaxTree->Evaluate(syntaxTree->root);
        if(syntaxTree->root->groundtruth!=UNKNOW) {
            cout << "FORMULA " << propertyid << " " << ((syntaxTree->root->groundtruth==TRUE)?"FALSE":"TRUE")<<endl;
            outresult << ((syntaxTree->root->groundtruth==TRUE)?'F':'T');
            continue;
        }
        syntaxTree->Universe(syntaxTree->root);
        syntaxTree->Get_DNF(syntaxTree->root);
        syntaxTree->Build_VWAA();
        syntaxTree->VWAA_Simplify();

        bool slice = syntaxTree->isNextFree();
//        SLICEPLACE = false;
        if(slice) {
            SLICEPLACE = SLICETRANSITION = true;
            //implement slice
            syntaxTree->getVisibleIterms();
            petri->implementSlice(syntaxTree->visibleIterms,cardinality);
            setGlobalValue(ptnet);
        }
        else {
            SLICEPLACE = SLICETRANSITION = false;
            //undoSlice
            petri->undoSlicePlace();
            petri->undoSliceTrans();
            setGlobalValue(ptnet);
        }
//        petri->printVisTransitions();

        GBA = new General;
        GBA->Build_GBA(*syntaxTree);
        GBA->Simplify();
        GBA->self_check();

        BA = new Buchi;
        BA->Build_BA(*GBA);
        BA->Simplify();
        BA->self_check();
        BA->Backward_chaining();
//        BA->PrintBuchi("BA->dot");

        SBA = new StateBuchi;
        SBA->Build_SBA(*BA);
        SBA->Simplify();
        SBA->Tarjan();
        SBA->Complete1();
        SBA->Add_heuristic();
        SBA->Complete2();
        SBA->self_check();
//        SBA->PrintStateBuchi();
        SBA->linkAtomics(syntaxTree->AT,cardinality);

        if (NUPN || SAFE || PINVAR || LONGBITPLACE) {
            bitgraph = new BitRG(ptnet, syntaxTree->AT);
//            BitRGNode *initnode = bitgraph->RGinitialnode();
//            bitgraph->Generate(initnode);
//            cout<<"STATE SPACE:"<<bitgraph->nodecount<<endl;
        } else {
            graph = new RG(ptnet, syntaxTree->AT);
//            RGNode *initnode = graph->RGinitialnode();
//            graph->Generate(initnode);
//            cout<<"STATE SPACE:"<<graph->nodecount<<endl;
        }

        //cout << "begin:ON-THE-FLY" << endl;
        ready2exit = false;
        string Techniques;
        bool dataOverBound;
        if (NUPN || SAFE || PINVAR || LONGBITPLACE) {
            Product_Automata<BitRGNode, BitRG> *product;
            product = new Product_Automata<BitRGNode, BitRG>(ptnet, bitgraph, SBA);

            product->ModelChecker(propertyid,each_run_time,Techniques);
            endtime = get_time();

            if(product->data_flag) {
                cout<<" NODECOUNT "<<bitgraph->nodecount<<" TIME "<<(endtime-starttime)
                    <<" MEM "<<product->get_current_mem()<<Techniques<<endl;
                int ret = product->getresult();
                outresult << (ret == -1 ? '?' : (ret == 0 ? 'F' : 'T'));
            }

            dataOverBound = product->data_flag?false:true;
            delete product;

        } else {
            Product_Automata<RGNode, RG> *product;
            product = new Product_Automata<RGNode, RG>(ptnet, graph, SBA);

            product->ModelChecker(propertyid,each_run_time,Techniques);
            endtime = get_time();

            if(product->data_flag) {
                cout<<" NODECOUNT "<<graph->nodecount<<" TIME "<<(endtime-starttime)
                    <<" MEM "<<product->get_current_mem()<<Techniques<<endl;
                int ret = product->getresult();
                outresult << (ret == -1 ? '?' : (ret == 0 ? 'F' : 'T'));
            }

            dataOverBound = product->data_flag?false:true;
            delete product;
        }

        //资源释放

        if (NUPN || SAFE || PINVAR || LONGBITPLACE) {
            delete bitgraph;
        } else {
            delete graph;
        }
        if(SLICEPLACE && PINVAR) {
            petri->undoPinvarSlicePlace();
        }
        else if(SLICEPLACE)
            petri->undoSlicePlace();
        if(SLICETRANSITION)
            petri->undoSliceTrans();
        delete syntaxTree;delete GBA;delete BA;delete SBA;

        endtime = get_time();
        total_left_time-=(unsigned short)(endtime-starttime);
        MallocExtension::instance()->ReleaseFreeMemory();

        if(dataOverBound) {
            LONGBITPLACE = true;
            if(PINVAR)
                petri->reComputeBound();
            i--;
            continue;
        }
    }
    outresult<<endl;
}

void CHECKLTL(Petri *ptnet,bool cardinality,int num) {
    BitRG *bitgraph;RG *graph;
    unsigned short each_run_time=300;

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
    syntaxTree.Evaluate(syntaxTree.root);
    if(syntaxTree.root->groundtruth!=UNKNOW) {
        cout << "FORMULA " << propertyid << " " << ((syntaxTree.root->groundtruth==TRUE)?"TRUE":"FALSE")<<endl;
    }
    syntaxTree.PrintTree();
    cout<<"-----------------------------------"<<endl;
    syntaxTree.Universe(syntaxTree.root);
    cout<<"after universe"<<endl;
    syntaxTree.PrintTree();
    cout<<"-----------------------------------"<<endl;

    syntaxTree.Get_DNF(syntaxTree.root);
    syntaxTree.Build_VWAA();
    syntaxTree.VWAA_Simplify();

    bool slice = syntaxTree.isNextFree();
    if(slice) {
        SLICETRANSITION = SLICEPLACE = true;
        //implement slice
        syntaxTree.getVisibleIterms();
        petri->implementSlice(syntaxTree.visibleIterms,cardinality);
        setGlobalValue(ptnet);
    }
    else {
        SLICETRANSITION = SLICEPLACE = false;
        //undoSlice
        petri->undoSlicePlace();
        petri->undoSliceTrans();
        setGlobalValue(ptnet);
    }

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
        SBA.linkAtomics(syntaxTree.AT,cardinality);
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
    string Techniques;
    bool dataOverBound;
    if (NUPN || SAFE || PINVAR || LONGBITPLACE) {
        Product_Automata<BitRGNode, BitRG> *product;
        product = new Product_Automata<BitRGNode, BitRG>(ptnet, bitgraph, &SBA);

        product->ModelChecker(propertyid,each_run_time,Techniques);

        if(product->data_flag) {
            cout<<" NODECOUNT "<<bitgraph->nodecount
                <<" MEM "<<product->get_current_mem()<<Techniques<<endl;
            int ret = product->getresult();
        }

        dataOverBound = product->data_flag?false:true;
        delete product;

    } else {
        Product_Automata<RGNode, RG> *product;
        product = new Product_Automata<RGNode, RG>(ptnet, graph, &SBA);

        product->ModelChecker(propertyid,each_run_time,Techniques);

        if(product->data_flag) {
            cout<<" NODECOUNT "<<graph->nodecount
                <<" MEM "<<product->get_current_mem()<<Techniques<<endl;
            int ret = product->getresult();
        }

        dataOverBound = product->data_flag?false:true;
        delete product;
    }


    if(SLICEPLACE && PINVAR) {
        petri->undoPinvarSlicePlace();
    }
    else if(SLICEPLACE)
        petri->undoSlicePlace();
    if(SLICETRANSITION)
        petri->undoSliceTrans();

    if (NUPN || SAFE || PINVAR || LONGBITPLACE) {
        delete bitgraph;
    } else {
        delete graph;
    }

    if(dataOverBound) {
        LONGBITPLACE = true;
        if(PINVAR)
            petri->reComputeBound();
    }
}

int main() {
    CHECKMEM();

    double starttime, endtime;
    starttime = get_time();

    CONSTRUCTPETRI();
    CHECKLTL(petri,1);
    CHECKLTL(petri,0);
    endtime = get_time();
    cout<<"RUNTIME "<<endtime-starttime<<endl;
    cout<<endl;
    delete petri;
    return 0;
}

int main0(int argc,char *argv[])
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