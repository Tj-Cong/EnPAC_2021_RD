//
// Created by hecong on 2020/7/30.
//
#include "set.h"
#include <vector>
using namespace std;

struct STNode;
/*transition structure of VWAA
 * if predicates is empty and destinations is empty too,
 * it indicates a transition (tt,tt);
 * if false_node is true,
 * it indicates this transition is non-sense;
 * */
typedef struct VWAA_delta
{
    set<string> predicates;               //conjunction of predicates
    set<const STNode *> destinations;     //conjunction of states
    VWAA_delta() {};
    VWAA_delta(string predi, STNode *dest) {
        if(predi != "")
            predicates.insert(predi);
        if(dest != NULL)
            destinations.insert(dest);
    }
    bool implies(const VWAA_delta &delta2) const;
} VWAA_delta;

bool operator == (const VWAA_delta &d1, const VWAA_delta &d2);
bool operator <= (const VWAA_delta &d1, const VWAA_delta &d2);

/*conjunction like operation on VWAA_delta
 * eg. d1=(q,GFq) d2=(p,Fp)
 * d1*d2=(q&&p,GFq&&Fp)
 * */
VWAA_delta operator * (const VWAA_delta &d1, const VWAA_delta &d2);

/*It is a set union operation
 * eg: l1={(p,tt)}, l2={(q,Ga)}; l1+l2={(p,tt),(q,Ga)}
 * */
vector<VWAA_delta> operator + (const vector<VWAA_delta> &l1, const vector<VWAA_delta> &l2);

/*This operation is like Cartesian product
 * eg: l1={(p,GFp),(tt,GFp&&Fp)};
 * l2={(!r&&q,G!r),(tt,F(q&&G!r))}
 * l1*l2={(p&&!r&&q,GFp&&G!r),(p,GFp&&F(q&&G!r)),(!r&&q,G!r&&GFp&&fp),(tt,GFp&&Fp&&F(q&&G!r))}
 * */
vector<VWAA_delta> operator * (const vector<VWAA_delta> &l1, const vector<VWAA_delta> &l2);