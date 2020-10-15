//
// Created by hecong on 2020/7/30.
//
#include "Syntax_Tree.h"

string DrawType(NodeType ntyp) {
    string character;
    switch (ntyp) {
        case ROOT:break;
        case PREDICATE:break;
        case NEG: character="!";break;
        case CONJUNC: character="&&";break;
        case DISJUNC: character="||";break;
        case NEXT: character="X";break;
        case ALWAYS: character="G";break;
        case EVENTUALLY: character="F";break;
        case U_OPER: character="U";break;
        case V_OPER: character="V";break;
        default:break;
    }
    return character;
}

Syntax_Tree::Syntax_Tree() {
    root = new STNode(ROOT);
    simplest = true;
    UID = 0;
}

Syntax_Tree::~Syntax_Tree() {
    Destroy(root);
}

void Syntax_Tree::Destroy(STNode *n) {
    if(n->nleft!=NULL)
        Destroy(n->nleft);
    if(n->nright!=NULL)
        Destroy(n->nright);
    delete n;
}

void Syntax_Tree::PrintTree() const {
    cout<<propertyid<<":"<<endl;
    PrintTree(root,0);
}

void Syntax_Tree::PrintTree(STNode *n, int depth) const{
    int i;
    char ch = ' ';
    if (n != NULL)
    {
        for (i = 0; i < depth; i++)
            cout << setw(3) << ch;
        switch (n->ntyp) {
            case ROOT:break;
            case PREDICATE: cout<<n->formula<<endl;break;
            case NEG: cout<<"!"<<endl;break;
            case CONJUNC: cout<<"&&"<<endl;break;
            case DISJUNC: cout<<"||"<<endl;break;
            case NEXT: cout<<"X"<<endl;break;
            case ALWAYS: cout<<"G"<<endl;break;
            case EVENTUALLY: cout<<"F"<<endl;break;
            case U_OPER: cout<<"U"<<endl;break;
            case V_OPER: cout<<"V"<<endl;break;
            default:break;
        }
        PrintTree(n->nleft, depth+1);
        PrintTree(n->nright, depth+1);
    }
}

void Syntax_Tree::ParseXML(char *filename, string &property, int number) {
    if(number<=0 || number>16) {
        cerr<<"Number must be between [1,16]"<<endl;
        exit(-1);
    }
    TiXmlDocument *doc = new TiXmlDocument(filename);
    if (!doc->LoadFile()) {
        cerr << doc->ErrorDesc() << endl;
    }
    TiXmlElement *root = doc->RootElement();
    if (root == NULL) {
        cerr << "Failed to load file: no root element!" << endl;
        doc->Clear();
    }
    TiXmlElement *p = root->FirstChildElement();
    for(int i=1; i<number; ++i) {
        p=p->NextSiblingElement();
    }
    string value = p->Value();
    TiXmlElement *id = p->FirstChildElement("id");
    property = this->propertyid = id->GetText();
    TiXmlElement *formula = p->FirstChildElement("formula");
    BuildTree(formula->FirstChildElement(),this->root);
    delete doc;
}

void Syntax_Tree::BuildTree(TiXmlElement *xmlnode, STNode* &stnode) {
    string nodename = xmlnode->Value();
    if(nodename == "all-paths") {
        BuildTree(xmlnode->FirstChildElement(),stnode->nleft);
    }
    else if(nodename == "negation") {
        stnode = new STNode(NEG);
        BuildTree(xmlnode->FirstChildElement(),stnode->nleft);
    }
    else if(nodename == "conjunction") {
        stnode = new STNode(CONJUNC);
        TiXmlElement *m = xmlnode->FirstChildElement();
        STNode *temp=stnode,*parent;
        while (m) {
            BuildTree(m,temp->nleft);
            m = m->NextSiblingElement();
            if(m) {
                temp->nright = new STNode(CONJUNC);
                parent = temp;
                temp = temp->nright;
            }
            else {
                parent->nright = temp->nleft;
                delete temp;
                break;
            }
        }
    }
    else if(nodename == "disjunction") {
        stnode = new STNode(DISJUNC);
        TiXmlElement *m = xmlnode->FirstChildElement();
        STNode *temp=stnode,*parent;
        while (m) {
            BuildTree(m,temp->nleft);
            m = m->NextSiblingElement();
            if(m) {
                temp->nright = new STNode(DISJUNC);
                parent = temp;
                temp = temp->nright;
            }
            else {
                parent->nright = temp->nleft;
                delete temp;
                break;
            }
        }
    }
    else if(nodename == "next") {
        stnode = new STNode(NEXT);
        BuildTree(xmlnode->FirstChildElement(),stnode->nleft);
    }
    else if(nodename == "globally") {
        stnode = new STNode(ALWAYS);
        BuildTree(xmlnode->FirstChildElement(),stnode->nleft);
    }
    else if(nodename == "finally") {
        stnode = new STNode(EVENTUALLY);
        BuildTree(xmlnode->FirstChildElement(),stnode->nleft);
    }
    else if(nodename == "until") {
        stnode = new STNode(U_OPER);
        TiXmlElement *m, *n;
        m = xmlnode->FirstChildElement();
        n = m->NextSiblingElement();
        string mValue = m->Value();
        string nValue = n->Value();
        if (mValue != "before")
        {
            cerr << "Error in XML file! The element until\'s first child is not before!" << endl;
            exit(-1);
        }
        if (nValue != "reach")
        {
            cerr << "Error in XML file! The element until\'s second child is not reach!" << endl;
            exit(-1);
        }
        BuildTree(m->FirstChildElement(),stnode->nleft);
        BuildTree(n->FirstChildElement(),stnode->nright);
    }
    else if(nodename == "is-fireable") {
        stnode = new STNode(PREDICATE);
        stnode->formula += "{";
        TiXmlElement *m;
        m = xmlnode->FirstChildElement();
        while(m) {
            string Value = m->Value();
            if (Value == "transition")
            {
                stnode->formula += m->GetText();
                stnode->formula += ",";
            }
            else
            {
                cerr << "Error in XML file! One of the element is-firability\'s child is not transition!" << endl;
                exit(-1);
            }
            m = m->NextSiblingElement();
        }
        stnode->formula += "}";
    }
    else if(nodename == "integer-le") {
        stnode = new STNode(PREDICATE);
        stnode->formula += "{";
        TiXmlElement *m, *n;
        m = xmlnode->FirstChildElement();
        n = m->NextSiblingElement();
        string mValue = m->Value();
        string nValue = n->Value();

        if (mValue == "tokens-count")
        {
            stnode->formula += "token-count(";
            TiXmlElement *left = m->FirstChildElement();
            while (left != NULL)
            {
                stnode->formula += left->GetText();
                stnode->formula += ",";
                left = left->NextSiblingElement();
            }
            stnode->formula += ")";
        }
        else if (mValue == "integer-constant")
        {
            stnode->formula += m->GetText();
        }
        else {
            cerr << "Error in XML file about the element integer-le!" << endl;
            exit(-1);
        }

        stnode->formula += "<=";

        if (nValue == "tokens-count")
        {
            stnode->formula += "token-count(";
            TiXmlElement *right = n->FirstChildElement();
            while (right != NULL)
            {
                stnode->formula += right->GetText();
                stnode->formula += ",";
                right = right->NextSiblingElement();
            }
            stnode->formula += ")";
        }
        else if (mValue == "integer-constant")
        {
            stnode->formula += n->GetText();
        }
        else {
            cerr << "Error in XML file about the element integer-le!" << endl;
            exit(-1);
        }
        stnode->formula += "}";
    }
    else {
        cerr<<"Can not recognize operator:"<<nodename<<endl;
        exit(-1);
    }
}

/*!(!a) = a;
 * !(a && b) = !a || !b
 * !(a || b) = !a && !b
 * !(Xa) = X!a
 * !(Fa) = G!a
 * !(Ga) = F!a
 * !(aUb) = !a V !b
 * !(aVb) = !a U !b
 * */
void Syntax_Tree::Push_Negation(STNode *n, STNode *nparent, bool right) {
    switch (n->ntyp) {
        case ROOT: {
            Push_Negation(n->nleft,n,0);
            break;
        }
        case PREDICATE: {
            if(n->formula == "true")
                n->formula = "false";
            else if(n->formula == "false")
                n->formula = "true";
            else if(n->formula[0] == '!') {
                n->formula = n->formula.substr(1);
            }
            else {
                n->formula = "!"+n->formula;
            }
            break;
        }
        case NEG: {
            if(right) {
                nparent->nright = n->nleft;
                delete n;
                Negation_Down(nparent->nright,nparent,1);
            }
            else {
                nparent->nleft = n->nleft;
                delete n;
                Negation_Down(nparent->nleft,nparent,0);
            }
            break;
        }
        case CONJUNC: {
            n->ntyp = DISJUNC;
            Push_Negation(n->nleft,n,0);
            Push_Negation(n->nright,n,1);
            break;
        }
        case DISJUNC: {
            n->ntyp = CONJUNC;
            Push_Negation(n->nleft,n,0);
            Push_Negation(n->nright,n,1);
            break;
        }
        case NEXT: {
            Push_Negation(n->nleft,n,0);
            break;
        }
        case ALWAYS: {
            n->ntyp = EVENTUALLY;
            Push_Negation(n->nleft,n,0);
            break;
        }
        case EVENTUALLY: {
            n->ntyp = ALWAYS;
            Push_Negation(n->nleft,n,0);
            break;
        }
        case U_OPER: {
            n->ntyp = V_OPER;
            Push_Negation(n->nleft,n,0);
            Push_Negation(n->nright,n,1);
            break;
        }
        case V_OPER: {
            n->ntyp = U_OPER;
            Push_Negation(n->nleft,n,0);
            Push_Negation(n->nright,n,1);
            break;
        }
        default: cerr<<"Can not recognize operator: "<<n->ntyp<<endl; break;
    }
}

void Syntax_Tree::Negation_Down(STNode *n, STNode *nparent, bool right) {
    if(n->ntyp == NEG) {
        if(right) {
            nparent->nright = n->nleft;
            delete n;
            Push_Negation(nparent->nright,nparent,1);
        }
        else {
            nparent->nleft = n->nleft;
            delete n;
            Push_Negation(nparent->nleft,nparent,0);
        }
    }
    else if(n->ntyp == PREDICATE)
        return;
    else if(n->ntyp == DISJUNC || n->ntyp == CONJUNC || n->ntyp == U_OPER || n->ntyp == V_OPER){
        Negation_Down(n->nleft,n,0);
        Negation_Down(n->nright,n,1);
    }
    else {
        Negation_Down(n->nleft,n,0);
    }
}

/*pure_even:
 * 1. Any formula of the form F(phi) is a pure eventuality formula
 * 2. Given pure eventuality formulas phi_1, phi_2, and an arbitrary formula y,
 * each of (phi_1 && phi_2), (phi_1 || phi_2), (phi_1 U y), (G phi_1), (X phi_1), (phi_1 V phi_2)
 * is also a pure eventuality formula
 *
 *pure_univ:
 * 1. Any formula of the form G(phi) is a purely universal
 * 2. Given purely universal formulas phi_1, phi_2, and an arbitrary formula y,
 * each of (phi_1 && phi_2), (phi_1 || phi_2), (phi_1 U phi_2), (F phi_1), (X phi_1), (phi_1 V phi_2)
 * is also a purely universal formula
 * */
void Syntax_Tree::Perfect_Node(STNode *n) {
    switch (n->ntyp) {
        case ROOT: {
            Perfect_Node(n->nleft);
            break;
        }
        case PREDICATE: {
            break;
        }
        case NEG: {
            cerr<<"The formula hasn't been transformed into Negation Normal Form!"<<endl;
            exit(-1);
            break;
        }
        case CONJUNC: {
            Perfect_Node(n->nleft);
            Perfect_Node(n->nright);
            n->formula = "("+n->nleft->formula +")&&("+n->nright->formula+")";
            n->pure_univ = n->nleft->pure_univ && n->nright->pure_univ;
            n->pure_even = n->nleft->pure_even && n->nright->pure_even;
            break;
        }
        case DISJUNC: {
            Perfect_Node(n->nleft);
            Perfect_Node(n->nright);
            n->formula = "("+n->nleft->formula +")||("+n->nright->formula+")";
            n->pure_univ = n->nleft->pure_univ && n->nright->pure_univ;
            n->pure_even = n->nleft->pure_even && n->nright->pure_even;
            break;
        }
        case NEXT: {
            Perfect_Node(n->nleft);
            n->formula = "X("+n->nleft->formula+")";
            n->pure_even = n->nleft->pure_even;
            n->pure_univ = n->nleft->pure_univ;
            break;
        }
        case ALWAYS: {
            Perfect_Node(n->nleft);
            n->formula = "G("+n->nleft->formula+")";
            n->pure_univ = true;
            n->pure_even = n->nleft->pure_even;
            break;
        }
        case EVENTUALLY: {
            Perfect_Node(n->nleft);
            n->formula = "F("+n->nleft->formula+")";
            n->pure_even = true;
            n->pure_univ = n->nleft->pure_univ;
            break;
        }
        case U_OPER: {
            Perfect_Node(n->nleft);
            Perfect_Node(n->nright);
            n->formula = "("+n->nleft->formula +")U("+n->nright->formula+")";
            n->pure_even = n->nleft->pure_even;
            n->pure_univ = n->nleft->pure_univ && n->nright->pure_univ;
            break;
        }
        case V_OPER: {
            Perfect_Node(n->nleft);
            Perfect_Node(n->nright);
            n->formula = "("+n->nleft->formula +")R("+n->nright->formula+")";
            n->pure_univ = n->nleft->pure_univ && n->nright->pure_univ;
            n->pure_even = n->nleft->pure_even && n->nright->pure_even;
            break;
        }
        default: cerr<<"Can not recognize operator: "<<n->ntyp<<endl; break;
    }
}

void Syntax_Tree::SimplifyLTL() {
    do {
        simplest = true;
        Perfect_Node(root);
        Rewrite(root);
    }while (simplest == false);
}
/*1. (a U b)&&(c U b) = (a && c) U b
 * 2. (a U b)||(a U c) = a U (b || c)
 * 3. F(a U b) = F b
 * 4. whenever b is a pure eventuality formula (a U b)=b, and F b=b
 * 5. whenever b is a pure universality formula (a V b)=b, and G b=b
 * */
void Syntax_Tree::Rewrite(STNode *n, STNode *nparent, bool right) {
    if(n->ntyp==CONJUNC && n->nleft->ntyp==U_OPER && n->nright->ntyp==U_OPER) {
        STNode *left_u=n->nleft,*right_u=n->nright;
        if(left_u->nright->formula == right_u->nright->formula) {
            STNode *p1,*p2;
            p1 = new STNode(U_OPER);
            p2 = new STNode(CONJUNC);
            p1->nleft = p2;
            p1->nright = left_u->nright;
            p2->nleft = left_u->nleft;
            p2->nright = right_u->nleft;
            delete left_u;
            delete right_u;
            if(right) {
                nparent->nright = p1;
                delete n;
            }
            else {
                nparent->nleft = p1;
                delete n;
            }
            Rewrite(p1->nleft,p1,0);
            Rewrite(p1->nright,p1,1);
            simplest = false;
        }
    }
    else if(n->ntyp==DISJUNC && n->nleft->ntyp==U_OPER && n->nright->ntyp==U_OPER) {
        STNode *left_u=n->nleft,*right_u=n->nright;
        if(left_u->nleft->formula == right_u->nleft->formula) {
            STNode *p1,*p2;
            p1 = new STNode(U_OPER);
            p2 = new STNode(DISJUNC);
            p1->nleft = left_u->nleft;
            p1->nright = p2;
            p2->nleft = left_u->nright;
            p2->nright = right_u->nright;
            delete left_u;
            delete right_u;
            if(right) {
                nparent->nright = p1;
                delete n;
            }
            else {
                nparent->nleft = p1;
                delete n;
            }
            Rewrite(p1->nleft,p1,0);
            Rewrite(p1->nright,p1,1);
            simplest = false;
        }
    }
    else if(n->ntyp==EVENTUALLY && n->nleft->ntyp==U_OPER) {
        STNode *u = n->nleft;
        n->nleft = u->nright;
        Destroy(u->nleft);
        delete u;
        Rewrite(n->nleft,n,0);
        simplest = false;
    }
    else if(n->ntyp==U_OPER && n->nright->pure_even) {
        if(right) {
            nparent->nright = n->nright;
            Destroy(n->nleft);
            delete n;
            Rewrite(nparent->nright,nparent,1);
        }
        else {
            nparent->nleft = n->nright;
            Destroy(n->nleft);
            delete n;
            Rewrite(nparent->nleft,nparent,0);
        }
        simplest = false;
    }
    else if(n->ntyp==EVENTUALLY && n->nleft->pure_even) {
        if(right) {
            nparent->nright = n->nleft;
            delete n;
            Rewrite(nparent->nright,nparent,1);
        }
        else {
            nparent->nleft = n->nleft;
            delete n;
            Rewrite(nparent->nleft,nparent,0);
        }
        simplest = false;
    }
    else if(n->ntyp==V_OPER && n->nright->pure_univ) {
        if(right) {
            nparent->nright = n->nright;
            Destroy(n->nleft);
            delete n;
            Rewrite(nparent->nright,nparent,1);
        }
        else {
            nparent->nleft = n->nright;
            Destroy(n->nleft);
            delete n;
            Rewrite(nparent->nleft,nparent,0);
        }
        simplest = false;
    }
    else if(n->ntyp==ALWAYS && n->nleft->pure_univ) {
        if(right) {
            nparent->nright = n->nleft;
            delete n;
            Rewrite(nparent->nright,nparent,1);
        }
        else {
            nparent->nleft = n->nleft;
            delete n;
            Rewrite(nparent->nleft,nparent,0);
        }
        simplest = false;
    }
    else if(n->ntyp==DISJUNC) {
        bool merge = false;
        if(n->nleft->ntyp==PREDICATE && n->nright->ntyp==PREDICATE) {
            if(n->nleft->formula[0]=='!') {
                string str = n->nleft->formula.substr(1);
                if(str == n->nright->formula) {
                    merge = true;
                }
            }
            else if(n->nright->formula[0]=='!') {
                string str = n->nright->formula.substr(1);
                if(str == n->nleft->formula) {
                    merge = true;
                }
            }
        }
        if(merge) {
            n->formula = "true";
            n->ntyp = PREDICATE;
            Destroy(n->nleft);
            Destroy(n->nright);
            n->nleft = n->nright = NULL;
        }
        else {
            Rewrite(n->nleft,n,0);
            Rewrite(n->nright,n,1);
        }
    }
    else {
        if(n->nleft)
            Rewrite(n->nleft,n,0);
        if(n->nright)
            Rewrite(n->nright,n,1);
    }
}

/*F phi = true U phi; G phi = false V phi
 * */
void Syntax_Tree::Universe(STNode *n) {
    if(n->ntyp == ROOT)
        Universe(n->nleft);
    else if(n->ntyp == NEG) {
        cerr<<"The formula hasn't been transformed into Negation Normal Form!"<<endl;
        exit(-1);
    }
    else if(n->ntyp == ALWAYS) {
        n->ntyp = V_OPER;
        STNode *p = new STNode(PREDICATE);
        p->formula = "false";
        n->nright = n->nleft;
        n->nleft = p;
        Universe(n->nright);
    }
    else if(n->ntyp == EVENTUALLY) {
        n->ntyp = U_OPER;
        n->Uid = AssignUID();
        STNode *p = new STNode(PREDICATE);
        p->formula = "true";
        n->nright = n->nleft;
        n->nleft = p;
        Universe(n->nright);
    }
    else if(n->ntyp == U_OPER) {
        n->Uid = AssignUID();
        Universe(n->nleft);
        Universe(n->nright);
    }
    else {
        if(n->nleft!=NULL)
            Universe(n->nleft);
        if(n->nright!=NULL)
            Universe(n->nright);
    }
}

void Syntax_Tree::Build_VWAA() {
    Build_VWAA(root);
}

void Syntax_Tree::Build_VWAA(STNode *n) {
    switch (n->ntyp) {
        case ROOT: {
            Build_VWAA(n->nleft);
            break;
        }
        case PREDICATE: {
            if(n->formula == "true") {
                VWAA_delta delta;
                n->transitions.push_back(delta);
            }
            else if(n->formula != "false") {
                VWAA_delta delta(n->formula,NULL);
                n->transitions.push_back(delta);
            }
            break;
        }
        case CONJUNC: {
            Build_VWAA(n->nleft);
            Build_VWAA(n->nright);
            n->transitions = n->nleft->transitions * n->nright->transitions;
            break;
        }
        case DISJUNC: {
            Build_VWAA(n->nleft);
            Build_VWAA(n->nright);
            n->transitions = n->nleft->transitions + n->nright->transitions;
            break;
        }
        case NEXT: {
            Build_VWAA(n->nleft);
            vector<set<const STNode *>>::iterator l;
            for(l=n->nleft->DNF.begin();l!=n->nleft->DNF.end();++l) {
                VWAA_delta delta;
                delta.destinations = *l;
                n->transitions.push_back(delta);
            }
            break;
        }
        case U_OPER:{
            Build_VWAA(n->nleft);
            Build_VWAA(n->nright);
            VWAA_delta delta("",n);
            vector<VWAA_delta> temp;
            temp.push_back(delta);
            n->transitions = n->nright->transitions + (n->nleft->transitions * temp);
            break;
        }
        case V_OPER:{
            Build_VWAA(n->nleft);
            Build_VWAA(n->nright);
            VWAA_delta delta("",n);
            vector<VWAA_delta> temp;
            temp.push_back(delta);
            n->transitions = n->nright->transitions * (n->nleft->transitions + temp);
            break;
        }
        default: {
            cerr<<"Can not recognize operator: "<<DrawType(n->ntyp)<<endl;
            exit(-2);
        }
    }
}

void Syntax_Tree::Get_DNF(STNode *n) {
    switch (n->ntyp) {
        case ROOT: {
            Get_DNF(n->nleft);
            break;
        }
        case PREDICATE: {
            set<const STNode *> temp;
            temp.insert(n);
            n->DNF.push_back(temp);
            break;
        }
        case U_OPER:
        case V_OPER:{
            Get_DNF(n->nleft);
            Get_DNF(n->nright);
            set<const STNode *> temp;
            temp.insert(n);
            n->DNF.push_back(temp);
            break;
        }
        case NEXT: {
            Get_DNF(n->nleft);
            set<const STNode *> temp;
            temp.insert(n);
            n->DNF.push_back(temp);
            break;
        }
        case CONJUNC: {
            Get_DNF(n->nleft);
            Get_DNF(n->nright);
            vector<set<const STNode *>>::iterator l;
            vector<set<const STNode *>>::iterator r;
            for(l=n->nleft->DNF.begin();l!=n->nleft->DNF.end();++l) {
                for(r=n->nright->DNF.begin();r!=n->nright->DNF.end();++r) {
                    set<const STNode *>temp;
                    temp = *l + *r;
                    n->DNF.push_back(temp);
                }
            }
            break;
        }
        case DISJUNC: {
            Get_DNF(n->nleft);
            Get_DNF(n->nright);
            vector<set<const STNode *>>::iterator l;
            vector<set<const STNode *>>::iterator r;
            for(l=n->nleft->DNF.begin();l!=n->nleft->DNF.end();++l) {
                n->DNF.push_back(*l);
            }
            for(r=n->nright->DNF.begin();r!=n->nright->DNF.end();++r) {
                n->DNF.push_back(*r);
            }
            break;
        }

        default: {
            cerr<<"Can not recognize operator: "<<DrawType(n->ntyp)<<endl;
            exit(-2);
        }
    }
}

void Syntax_Tree::VWAA_Simplify() {
    VWAA_Simplify(root);
}

/*Simplify VWAA:
 * if a transition t1 implies a transition t2, then t2 can be removed
 * t1=(q,a1,q1) implies t2=(q,a2,q2) if a2 is sunseteq of a1 and q1 is subseteq of q2
 * Notice: if a1=m&&n&&k, a2=m&&n, a1 is subseqeq of a2;
 * if q1=GFp&&Fp, q2=GFp, q2 is subseteq of q1
 * */
void Syntax_Tree::VWAA_Simplify(STNode *n) {
    switch (n->ntyp) {
        case ROOT: {
            VWAA_Simplify(n->nleft);
            break;
        }
        case PREDICATE: break;
        case NEXT: {
            VWAA_Simplify(n->nleft);
            Transition_Simplify(n);
            break;
        }
        case U_OPER:
        case V_OPER:
        case CONJUNC:
        case DISJUNC: {
            VWAA_Simplify(n->nleft);
            VWAA_Simplify(n->nright);
            Transition_Simplify(n);
            break;
        }
        default: {
            cerr<<"Can not recognize operator: "<<DrawType(n->ntyp)<<endl;
            exit(-2);
        }
    }
}

void Syntax_Tree::Transition_Simplify(STNode *n) {
    bool changed;
    vector<VWAA_delta>::iterator delta1,delta2;
    do {
        changed = false;
        for(delta1=n->transitions.begin();delta1!=n->transitions.end();++delta1) {
            for(delta2=n->transitions.begin();delta2!=n->transitions.end();++delta2) {
                if(delta1!=delta2 && delta1->implies(*delta2)) {
                    n->transitions.erase(delta2);
                    changed = true;
                    break;
                }
            }
            if(changed)
                break;
        }
    }while(changed);
}

int Syntax_Tree::AssignUID() {
    return ++UID;
}
