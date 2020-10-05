//
// Created by tvtaqa on 2020/1/2.
//
#include <cstring>
#include <cmath>
#include "rref.h"

ComputeRref::ComputeRref(int r, int c, float **M) {
    row = r;
    col = c;
    Matrix = M;
//    Matrix = new float *[row];
//    for (int i = 0; i < row; i++) {
//        Matrix[i] = new float[col];
//        memset(Matrix[i], 0, sizeof(float) * col);
//    }
//    for (int i = 0; i < row; i++) {
//        for (int j = 0; j < col; j++)
//            Matrix[i][j] = M[i][j];
//    }
}

void ComputeRref::rref() {
    int manvarcount = 0;
    for (int j = 0; j < col && manvarcount < row; j++) {
        if (judgezero(Matrix[manvarcount][j]))
            change(manvarcount, j);
        //如果交换完后 还是0 则说明这一列全是0
        if (judgezero(Matrix[manvarcount][j]))
            continue;
        else {

            //从主元下方全变0
            for (int t = manvarcount + 1; t < row; t++) {
                //判断是否是0
                if (judgezero(Matrix[t][j]))
                    continue;
                else {
                    float muti = Matrix[t][j] / Matrix[manvarcount][j];
                    for (int k = 0; k < col; k++) {
                        Matrix[t][k] = Matrix[t][k] - muti * Matrix[manvarcount][k];
                    }
                }
            }
            manvarcount++;
        }
    }
}

void ComputeRref::change(int count, int j) {
    //从j行开始寻找第一个不为0 的进行交换
    for (int r = count + 1; r < row; r++) {
        if (!judgezero(Matrix[r][j])) {
            for (int i = 0; i < col; i++) {
                float tmp = Matrix[count][i];
                Matrix[count][i] = Matrix[r][i];
                Matrix[r][i] = tmp;
            }
            break;
        } else {
            continue;
        }
    }
}

bool ComputeRref::judgezero(float num) {
    if (fabs(num) < 0.0000001)return 1;
    else return 0;
}


ComputeRref::~ComputeRref() {}

