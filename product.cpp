//
// Created by hecong on 19-5-6.
//
#include "product.h"

bool timeflag;
void  sig_handler(int num)
{
    //printf("time out .\n");
    timeflag = false;
    ready2exit = true;
}
