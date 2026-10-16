#ifndef  ASTAR_H
#define  ASTAR_H

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <list>
#include <stack>
#include <assert.h>
#include <graphics.h>
#include <conio.h>
#include <windows.h> 
#include <Mmsystem.h> 
#include <time.h>
#pragma comment(lib,"winmm.lib")
#pragma pack(1)
using namespace std;

#define NEEDSUSPEND   1

#define  IGNORE_CORNER false

#define  cellSize  28

typedef struct stPoint {
    int x;
    int y;
    int F;  // F = G + H
    int G;
    int H;
    stPoint* pstPre;
}stPoint;

#endif