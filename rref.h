//
// Created by tvtaqa on 2020/1/13.
//

#ifndef ENPAC_2020_3_0_RREF_H
#define ENPAC_2020_3_0_RREF_H
class ComputeRref {
public:
    int row;
    int col;
    float **Matrix;
public:
    ComputeRref(int r, int c, float **M);

    void rref();

    void change(int count, int j);

    bool judgezero(float num);

    ~ComputeRref();
};
#endif //ENPAC_2020_3_0_RREF_H
