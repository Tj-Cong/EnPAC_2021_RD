//
// Created by hecong on 2020/7/30.
//
#include "VWAA.h"


/* t1=(q,a1,q1) implies t2=(q,a2,q2) if a2 <= a1 and q1 <= q2
 * Notice: if a1=m&&n&&k, a2=m&&n, a1 <= a2;
 * if q1=GFp&&Fp, q2=GFp, q2 <= q1.
 * In other words,
 * if this->predicates <= delta2->predicates
 * && this->destination <= delta2->destination
 * this implies delta2
 * */
bool VWAA_delta::implies(const VWAA_delta &delta2) const {
    return (SUBSETEQ(delta2.predicates, this->predicates)
           && SUBSETEQ(delta2.destinations, this->destinations));
}

bool operator == (const VWAA_delta &d1, const VWAA_delta &d2) {
    return (d1.predicates == d2.predicates && d1.destinations == d2.destinations);
}

/*d1<=d2:
 * d1=(e,a',e''), d2=(e,a,e'), a<=a',e''<=e'
 * In other words:
 * d1<=d2 <=>  d1.predicates <= d2.predicates && d1.destinations <= d2.destinations
 * */
bool operator <= (const VWAA_delta &d1, const VWAA_delta &d2) {
    return (SUBSETEQ(d2.predicates,d1.predicates) && SUBSETEQ(d2.destinations,d1.destinations));
}

/*conjunction like operation on VWAA_delta
 * eg. d1=(q,GFq) d2=(p,Fp)
 * d1*d2=(q&&p,GFq&&Fp)
 * */
VWAA_delta operator * (const VWAA_delta &d1, const VWAA_delta &d2) {
    VWAA_delta result;
    result.predicates = d1.predicates + d2.predicates;
    result.destinations = d1.destinations + d2.destinations;
    return result;
}

/*It is a set union operation
 * eg: l1={(p,tt)}, l2={(q,Ga)}; l1+l2={(p,tt),(q,Ga)}
 * */
vector<VWAA_delta> operator + (const vector<VWAA_delta> &l1, const vector<VWAA_delta> &l2) {
    vector<VWAA_delta> result = l1;
    vector<VWAA_delta>::const_iterator item1;
    vector<VWAA_delta>::const_iterator item2;
    for(item2=l2.begin();item2!=l2.end();++item2) {
        bool exist = false;
        for(item1=l1.begin();item1!=l1.end();++item1) {
            if(*item1 == *item2) {
                exist = true;
                break;
            }
        }
        if(!exist) {
            result.push_back(*item2);
        }
    }
    return result;
}

/*This operation is like Cartesian product
 * eg: l1={(p,GFp),(tt,GFp&&Fp)};
 * l2={(!r&&q,G!r),(tt,F(q&&G!r))}
 * l1*l2={(p&&!r&&q,GFp&&G!r),(p,GFp&&F(q&&G!r)),(!r&&q,G!r&&GFp&&fp),(tt,GFp&&Fp&&F(q&&G!r))}
 * */
vector<VWAA_delta> operator * (const vector<VWAA_delta> &l1, const vector<VWAA_delta> &l2) {
    vector<VWAA_delta> result;
    vector<VWAA_delta>::const_iterator item1;
    vector<VWAA_delta>::const_iterator item2;
    for(item1=l1.begin();item1!=l1.end();++item1) {
        for(item2=l2.begin();item2!=l2.end();++item2){
            result.push_back((*item1)*(*item2));
        }
    }
    return result;
}