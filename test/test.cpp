#include <graphics.h>
#include <conio.h>
#include <windows.h> 
#include <Mmsystem.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#pragma comment(lib,"winmm.lib")
#pragma pack(1)
using namespace std;


// customize£º
const int animate = 1;
const int flash = 1;
const int hide = 0;
const int delay = 0;
const int between = 40;
const int maxtime = 90;
int steps = 40;


// adjustment£º
const int n = 15;
const int cellSize = 28;
const int spacing = 2;
const int spacings = 5;
const int linew = 2;
const int lines = 2;
const int over = 1;
const int x1 = 520, yy1 = 420, x2 = 590, y2 = 450;
const int x3 = 250, y3 = 200, x4 = 390, y4 = 240;
const int x5 = 520, y5 = 150, x6 = 600, y6 = 170;
const int x7 = 520, y7 = 280, x8 = 600, y8 = 300;
int BEGIN_X = 0;
int BEGIN_Y = 0;
int END_X = 14;
int END_Y = 14;
int rege = 1;


// variable£º
int x00, y00;
int x, y;
int xx = 0, yy = 0;
int wayx = 1, wayy = 1;
int matrix[n][n];
int windowSize = n * cellSize + (n - 1) * spacing + spacings * 2;
int movex = (480 - windowSize) / 2;
int movey = (480 - windowSize) / 2;
int timeuse;
int key = 0;
int sheep = 1;
int out = 0;


#include "Astar.h"
stPoint   stMapPointAll[n][n] = { 0 };
list<stPoint*> OpenList;
list<stPoint*> CloseList;
void initMap()
{
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
        {
            stMapPointAll[i][j].x = i;
            stMapPointAll[i][j].y = j;
            stMapPointAll[i][j].G = 0;
            stMapPointAll[i][j].H = (abs(i - END_X) + abs(j - END_Y)) * 10;
            stMapPointAll[i][j].F = stMapPointAll[i][j].G + stMapPointAll[i][j].H;
            stMapPointAll[i][j].pstPre = NULL;
        }
    OpenList.clear();
    CloseList.clear();
}
stPoint* findMinFPoint()
{
    int iFmin = 10000;
    stPoint* pstPoint = NULL;

    if (!OpenList.empty())
    {
        for (list<stPoint*>::iterator it = OpenList.begin();
            it != OpenList.end(); it++)
        {
            assert((*it)->F < 10000);
            if ((*it)->F < iFmin)
            {
                pstPoint = *it;
                iFmin = (*it)->F;
            }
        }
    }
    return pstPoint;

}
bool isCanreach(stPoint* pstPoint, stPoint* pstCurPoint)
{
    if (matrix[pstPoint->x][pstPoint->y] == 1)
        return false;
    if (pstPoint->x == pstCurPoint->x && pstPoint->y == pstCurPoint->y)
        return false;
    if (abs(pstCurPoint->x - pstPoint->x) + abs(pstCurPoint->y - pstPoint->y) == 1)
        return true;
    else
    {
        if (matrix[pstPoint->x][pstCurPoint->y] == 0 && matrix[pstCurPoint->x][pstPoint->y] == 0)
            return true;
        else
            return IGNORE_CORNER;
    }
}
void getSurroundPoints(stPoint* pstCurPoint, vector<stPoint*>& surroundPoints)
{
    for (int x = pstCurPoint->x - 1; x <= pstCurPoint->x + 1; x++)
        for (int y = pstCurPoint->y - 1; y <= pstCurPoint->y + 1; y++)
        {
            if (x >= 0 && x < n && y >= 0 && y < n)
            {
                if (isCanreach(&stMapPointAll[x][y], pstCurPoint))
                {
                    surroundPoints.push_back(&stMapPointAll[x][y]);
                }
            }
        }
}
bool isInCloseList(stPoint* pstPoint)
{
    for (list<stPoint*>::iterator it = CloseList.begin();
        it != CloseList.end(); it++)
    {
        if (*it == pstPoint)
            return true;
    }
    return false;
}
bool isInOpenList(stPoint* pstPoint)
{
    for (list<stPoint*>::iterator it = OpenList.begin();
        it != OpenList.end(); it++)
    {
        if (*it == pstPoint)
            return true;
    }
    return false;
}
void drawPath()
{
    for (int col = 0; col < n; col++)
    {
        for (int row = 0; row < n; row++)
        {
            x00 = movex + spacings + col * (cellSize + spacing);
            y00 = movey + spacings + row * (cellSize + spacing);
            if ((row == BEGIN_X && col == BEGIN_Y) || ((row == END_X) && (col == END_Y)))
            {
                setfillstyle(BS_SOLID);
                setfillcolor(YELLOW);
                solidrectangle(x00, y00, x00 + cellSize, y00 + cellSize);
            }
        }
    }
    stPoint* pstPoint = stMapPointAll[END_X][END_Y].pstPre;
    vector<stPoint*> PathVec;
    PathVec.clear();
    while (pstPoint != NULL)
    {
        if (pstPoint->x != BEGIN_X || pstPoint->y != BEGIN_Y)
        {
            PathVec.push_back(pstPoint);
        }
        pstPoint = pstPoint->pstPre;
    }
    if (!PathVec.empty())
    {
        for (vector<stPoint*>::const_reverse_iterator   it = PathVec.crbegin();
            it != PathVec.crend(); it++)
        {
            setfillstyle(BS_SOLID);
            setfillcolor(DARKGRAY);

            x00 = movex + spacings + ((*it)->x) * (cellSize + spacing);
            y00 = movey + spacings + ((*it)->y) * (cellSize + spacing);
            solidrectangle(y00, x00, y00 + cellSize, x00 + cellSize);
        }
    }
}
void value()
{
    stPoint* pstCurPoint = NULL;
    int iFindPathFlag = 0;
begin:
    initMap();
    OpenList.push_back(&stMapPointAll[BEGIN_X][BEGIN_Y]);
    while (!OpenList.empty())
    {
        pstCurPoint = findMinFPoint();
        assert(NULL != pstCurPoint);
        OpenList.remove(pstCurPoint);
        CloseList.push_back(pstCurPoint);
        vector<stPoint* > surroundPoints;
        surroundPoints.clear();
        getSurroundPoints(pstCurPoint, surroundPoints);
        if (!surroundPoints.empty())
        {
            for (vector<stPoint*>::iterator it = surroundPoints.begin();
                it != surroundPoints.end(); it++)
            {
                if (isInCloseList(*it))
                    ;
                else if (!isInOpenList(*it))
                {
                    (*it)->pstPre = pstCurPoint;
                    int iTempG = (abs(pstCurPoint->x - (*it)->x) + abs(pstCurPoint->y - (*it)->y)) == 1 ? 10 : 14;
                    (*it)->G = iTempG + pstCurPoint->G;
                    (*it)->F = (*it)->G + (*it)->H;
                    OpenList.push_back(*it);
                }
                else
                {
                    int iTempG = (abs(pstCurPoint->x - (*it)->x) + abs(pstCurPoint->y - (*it)->y)) == 1 ? 10 : 14;
                    if (iTempG + pstCurPoint->G < (*it)->G)
                    {
                        (*it)->G = iTempG + pstCurPoint->G;
                        (*it)->F = (*it)->G + (*it)->H;
                        (*it)->pstPre = pstCurPoint;
                    }
                }
            }
        }
        if (isInOpenList(&stMapPointAll[END_X][END_Y]))
        {
            iFindPathFlag = 1;
            printf("the path is found \n");
            break;
        }
    }
    if (0 == iFindPathFlag)
        goto begin;
    else
        drawPath();
}
void value2()
{
    stPoint* pstCurPoint = NULL;
    int iFindPathFlag = 0;
    initMap();
    OpenList.push_back(&stMapPointAll[BEGIN_X][BEGIN_Y]);
    while (!OpenList.empty())
    {
        pstCurPoint = findMinFPoint();
        assert(NULL != pstCurPoint);
        OpenList.remove(pstCurPoint);
        CloseList.push_back(pstCurPoint);
        vector<stPoint* > surroundPoints;
        surroundPoints.clear();
        getSurroundPoints(pstCurPoint, surroundPoints);
        if (!surroundPoints.empty())
        {
            for (vector<stPoint*>::iterator it = surroundPoints.begin();
                it != surroundPoints.end(); it++)
            {
                if (isInCloseList(*it))
                    ;
                else if (!isInOpenList(*it))
                {
                    (*it)->pstPre = pstCurPoint;
                    int iTempG = (abs(pstCurPoint->x - (*it)->x) + abs(pstCurPoint->y - (*it)->y)) == 1 ? 10 : 14;
                    (*it)->G = iTempG + pstCurPoint->G;
                    (*it)->F = (*it)->G + (*it)->H;
                    OpenList.push_back(*it);
                }
                else
                {
                    int iTempG = (abs(pstCurPoint->x - (*it)->x) + abs(pstCurPoint->y - (*it)->y)) == 1 ? 10 : 14;
                    if (iTempG + pstCurPoint->G < (*it)->G)
                    {
                        (*it)->G = iTempG + pstCurPoint->G;
                        (*it)->F = (*it)->G + (*it)->H;
                        (*it)->pstPre = pstCurPoint;
                    }
                }
            }
        }
        if (isInOpenList(&stMapPointAll[END_X][END_Y]))
        {
            iFindPathFlag = 1;
            printf("the path is found \n");
            break;
        }
    }
    if (0 == iFindPathFlag)
        rege = 1;
    else
        rege = 0;
}

void title()
{
    PlaySound(_T("music.wav"), NULL, SND_FILENAME | SND_ASYNC);

    setbkcolor(WHITE);
    cleardevice();
    settextcolor(BLUE);

    IMAGE img;
    loadimage(&img, _T("bkimg.jpg"), 640, 480, true);
    putimage(0, 0, &img);
    setbkmode(TRANSPARENT);

    settextstyle(25, 0, _T("ËÎÌå"));
    RECT r2 = { 0,250,640,480 };
    
    RECT r3 = { 0,350,640,480 };
    
    IMAGE bk;
    getimage(&bk, 0, 0, 640, 480);
    IMAGE bkim;
    getimage(&bkim, 182, 200, 640, 305);
    IMAGE bksh;
    getimage(&bksh, 0, 300, 222, 480);

    BeginBatchDraw();

    int vertical = 480;

    for (int i = 0; i < 35; i++)
    {
        putimage(0, 0, &bk);

        IMAGE img3;
        loadimage(&img3, _T("yuantu.jpg"), 260, 240, true);
        IMAGE img4;
        loadimage(&img4, _T("yanma.jpg"), 260, 240, true);

        putimage(-10, vertical, 260, 240, &img4, 0, 0, SRCAND);
        putimage(-10, vertical, 260, 240, &img3, 0, 0, SRCPAINT);

        FlushBatchDraw();
        vertical -= 6;
        Sleep(1);
    }
    fflush(stdin);

    IMAGE duitu;
    loadimage(&duitu, _T("duitu.jpg"), 94, 94, true);
    IMAGE duiyanma;
    loadimage(&duiyanma, _T("duiyanma.jpg"), 94, 94, true);
    IMAGE guodu;
    getimage(&guodu, 0, 0, 640, 480);

    settextstyle(25, 0, _T("Algerian"));
    RECT r = { 5,300,320,480 };
    drawtext(_T("  Generating  Your  Maps ..."), &r, DT_CENTER | DT_VCENTER );
    FlushBatchDraw();

    while (1)
    {
        while (rege == 1)
        {
            srand(time(NULL));
            for (int row = 0; row < n; row++)
            {
                for (int col = 0; col < n; col++)
                {
                    if (rand() % 4 == 0)
                        matrix[row][col] = 1;
                    else
                        matrix[row][col] = 0;
                }
            }
            value2();
        }

        putimage(0, 0, &bk);
        putimage(0, 300, &bksh);
        IMAGE t1;
        loadimage(&t1, _T("t1.jpg"), 260, 240, true);
        IMAGE yyy1;
        loadimage(&yyy1, _T("y1.jpg"), 260, 240, true);
        putimage(-10, vertical + 4, 260, 240, &yyy1, 0, 0, SRCAND);
        putimage(-10, vertical + 4, 260, 240, &t1, 0, 0, SRCPAINT);
        Sleep(between);
        FlushBatchDraw();

        putimage(0, 0, &bk);
        putimage(0, 300, &bksh);
        IMAGE t2;
        loadimage(&t2, _T("t2.jpg"), 260, 240, true);
        IMAGE y2;
        loadimage(&y2, _T("y2.jpg"), 260, 240, true);
        putimage(-10, vertical + 4, 260, 240, &y2, 0, 0, SRCAND);
        putimage(-10, vertical + 4, 260, 240, &t2, 0, 0, SRCPAINT);
        Sleep(between);
        FlushBatchDraw();

        putimage(0, 0, &bk);
        putimage(0, 300, &bksh);
        IMAGE t3;
        loadimage(&t3, _T("t3.jpg"), 260, 240, true);
        IMAGE y3;
        loadimage(&y3, _T("y3.jpg"), 260, 240, true);
        putimage(-10, vertical + 4, 260, 240, &y3, 0, 0, SRCAND);
        putimage(-10, vertical + 4, 260, 240, &t3, 0, 0, SRCPAINT);
        Sleep(between);
        FlushBatchDraw();

        putimage(0, 0, &bk);
        putimage(0, 300, &bksh);
        IMAGE t4;
        loadimage(&t4, _T("t4.jpg"), 260, 240, true);
        IMAGE y4;
        loadimage(&y4, _T("y4.jpg"), 260, 240, true);
        putimage(-10, vertical + 4, 260, 240, &y4, 0, 0, SRCAND);
        putimage(-10, vertical + 4, 260, 240, &t4, 0, 0, SRCPAINT);
        Sleep(between);
        FlushBatchDraw();

        if (_kbhit())
        {
            key = _getch();
            putimage(182, 200, &bkim);
            switch (key)
            {
            case '1':
                sheep = 1;
                putimage(193, 208, 94, 94, &duiyanma, 0, 0, SRCAND);
                putimage(193, 208, 94, 94, &duitu, 0, 0, SRCPAINT);
                getimage(&bk, 0, 0, 640, 480);
                break;
            case '2':
                
                sheep = 2;
                putimage(306, 208, 94, 94, &duiyanma, 0, 0, SRCAND);
                putimage(306, 208, 94, 94, &duitu, 0, 0, SRCPAINT);
                getimage(&bk, 0, 0, 640, 480);
                break;
            case '3':
                
                sheep = 3;
                putimage(412, 208, 94, 94, &duiyanma, 0, 0, SRCAND);
                putimage(412, 208, 94, 94, &duitu, 0, 0, SRCPAINT);
                getimage(&bk, 0, 0, 640, 480);
                break;
            case '4':
                
                sheep = 4;
                putimage(523, 208, 94, 94, &duiyanma, 0, 0, SRCAND);
                putimage(523, 208, 94, 94, &duitu, 0, 0, SRCPAINT);
                getimage(&bk, 0, 0, 640, 480);
                break;
            case 'p':
            case 'P':
                key = 1;
                out = 1;
                break;
            default:
                out = 1;
                break;
            }
        }
        if (out == 1)
            break;

        putimage(0, 0, &bk);
        putimage(0, 300, &bksh);
        IMAGE t5;
        loadimage(&t5, _T("t5.jpg"), 260, 240, true);
        IMAGE y5;
        loadimage(&y5, _T("y5.jpg"), 260, 240, true);
        putimage(-10, vertical + 4, 260, 240, &y5, 0, 0, SRCAND);
        putimage(-10, vertical + 4, 260, 240, &t5, 0, 0, SRCPAINT);
        Sleep(between);
        FlushBatchDraw();

        putimage(0, 0, &bk);
        putimage(0, 300, &bksh);
        IMAGE t6;
        loadimage(&t6, _T("t6.jpg"), 260, 240, true);
        IMAGE y6;
        loadimage(&y6, _T("y6.jpg"), 260, 240, true);
        putimage(-10, vertical + 4, 260, 240, &y6, 0, 0, SRCAND);
        putimage(-10, vertical + 4, 260, 240, &t6, 0, 0, SRCPAINT);
        Sleep(between);
        FlushBatchDraw();

        putimage(0, 0, &bk);
        putimage(0, 300, &bksh);
        IMAGE t7;
        loadimage(&t7, _T("t7.jpg"), 260, 240, true);
        IMAGE y7;
        loadimage(&y7, _T("y7.jpg"), 260, 240, true);
        putimage(-10, vertical + 4, 260, 240, &y7, 0, 0, SRCAND);
        putimage(-10, vertical + 4, 260, 240, &t7, 0, 0, SRCPAINT);
        Sleep(between);
        FlushBatchDraw();

        putimage(0, 0, &bk);
        putimage(0, 300, &bksh);
        IMAGE t8;
        loadimage(&t8, _T("t8.jpg"), 260, 240, true);
        IMAGE y8;
        loadimage(&y8, _T("y8.jpg"), 260, 240, true);
        putimage(-10, vertical + 4, 260, 240, &y8, 0, 0, SRCAND);
        putimage(-10, vertical + 4, 260, 240, &t8, 0, 0, SRCPAINT);
        Sleep(between);
        FlushBatchDraw();

        if (_kbhit())
        {
            key = _getch();
            putimage(182, 200, &bkim);
            switch (key)
            {
            case '1':
                sheep = 1;
                putimage(193, 208, 94, 94, &duiyanma, 0, 0, SRCAND);
                putimage(193, 208, 94, 94, &duitu, 0, 0, SRCPAINT);
                getimage(&bk, 0, 0, 640, 480);
                break;
            case '2':
                
                sheep = 2;
                putimage(306, 208, 94, 94, &duiyanma, 0, 0, SRCAND);
                putimage(306, 208, 94, 94, &duitu, 0, 0, SRCPAINT);
                getimage(&bk, 0, 0, 640, 480);
                break;
            case '3':
                
                sheep = 3;
                putimage(412, 208, 94, 94, &duiyanma, 0, 0, SRCAND);
                putimage(412, 208, 94, 94, &duitu, 0, 0, SRCPAINT);
                getimage(&bk, 0, 0, 640, 480);
                break;
            case '4':
                
                sheep = 4;
                putimage(523, 208, 94, 94, &duiyanma, 0, 0, SRCAND);
                putimage(523, 208, 94, 94, &duitu, 0, 0, SRCPAINT);
                getimage(&bk, 0, 0, 640, 480);
                break;
            case 'p':
            case 'P':
                key = 1;
                out = 1;
                break;
            default:
                out = 1;
                break;
            }
        }
        if (out == 1)
            break;
        
        putimage(0, 0, &bk);
        putimage(0, 300, &bksh);
        IMAGE t9;
        loadimage(&t9, _T("t9.jpg"), 260, 240, true);
        IMAGE y9;
        loadimage(&y9, _T("y9.jpg"), 260, 240, true);
        putimage(-10, vertical + 4, 260, 240, &y9, 0, 0, SRCAND);
        putimage(-10, vertical + 4, 260, 240, &t9, 0, 0, SRCPAINT);
        Sleep(between);
        FlushBatchDraw();

        putimage(0, 0, &bk);
        putimage(0, 300, &bksh);
        IMAGE t10;
        loadimage(&t10, _T("t10.jpg"), 260, 240, true);
        IMAGE y10;
        loadimage(&y10, _T("y10.jpg"), 260, 240, true);
        putimage(-10, vertical + 4, 260, 240, &y10, 0, 0, SRCAND);
        putimage(-10, vertical + 4, 260, 240, &t10, 0, 0, SRCPAINT);
        Sleep(between);
        FlushBatchDraw();

        putimage(0, 0, &bk);
        putimage(0, 300, &bksh);
        IMAGE t11;
        loadimage(&t11, _T("t11.jpg"), 260, 240, true);
        IMAGE y11;
        loadimage(&y11, _T("y11.jpg"), 260, 240, true);
        putimage(-10, vertical + 4, 260, 240, &y11, 0, 0, SRCAND);
        putimage(-10, vertical + 4, 260, 240, &t11, 0, 0, SRCPAINT);
        Sleep(between);
        FlushBatchDraw();

        putimage(0, 0, &bk);
        putimage(0, 300, &bksh);
        IMAGE t12;
        loadimage(&t12, _T("t12.jpg"), 260, 240, true);
        IMAGE y12;
        loadimage(&y12, _T("y12.jpg"), 260, 240, true);
        putimage(-10, vertical + 4, 260, 240, &y12, 0, 0, SRCAND);
        putimage(-10, vertical + 4, 260, 240, &t12, 0, 0, SRCPAINT);
        Sleep(between);
        FlushBatchDraw();

        if (_kbhit())
        {
            key = _getch();
            putimage(182, 200, &bkim);
            switch (key)
            {
            case '1':
                sheep = 1;
                putimage(193, 208, 94, 94, &duiyanma, 0, 0, SRCAND);
                putimage(193, 208, 94, 94, &duitu, 0, 0, SRCPAINT);
                getimage(&bk, 0, 0, 640, 480);
                break;
            case '2':
                
                sheep = 2;
                putimage(306, 208, 94, 94, &duiyanma, 0, 0, SRCAND);
                putimage(306, 208, 94, 94, &duitu, 0, 0, SRCPAINT);
                getimage(&bk, 0, 0, 640, 480);
                break;
            case '3':
                
                sheep = 3;
                putimage(412, 208, 94, 94, &duiyanma, 0, 0, SRCAND);
                putimage(412, 208, 94, 94, &duitu, 0, 0, SRCPAINT);
                getimage(&bk, 0, 0, 640, 480);
                break;
            case '4':
                
                sheep = 4;
                putimage(523, 208, 94, 94, &duiyanma, 0, 0, SRCAND);
                putimage(523, 208, 94, 94, &duitu, 0, 0, SRCPAINT);
                getimage(&bk, 0, 0, 640, 480);
                break;
            case 'p':
            case 'P':
                key = 1;
                out = 1;
                break;
            default:
                out = 1;
                break;
            }
        }
        if (out == 1)
            break;

        putimage(0, 0, &bk);
        putimage(0, 300, &bksh);
        IMAGE t13;
        loadimage(&t13, _T("t13.jpg"), 260, 240, true);
        IMAGE y13;
        loadimage(&y13, _T("y13.jpg"), 260, 240, true);
        putimage(-10, vertical + 4, 260, 240, &y13, 0, 0, SRCAND);
        putimage(-10, vertical + 4, 260, 240, &t13, 0, 0, SRCPAINT);
        Sleep(between);
        FlushBatchDraw();

        putimage(0, 0, &bk);
        putimage(0, 300, &bksh);
        IMAGE t14;
        loadimage(&t14, _T("t14.jpg"), 260, 240, true);
        IMAGE y14;
        loadimage(&y14, _T("y14.jpg"), 260, 240, true);
        putimage(-10, vertical + 4, 260, 240, &y14, 0, 0, SRCAND);
        putimage(-10, vertical + 4, 260, 240, &t14, 0, 0, SRCPAINT);
        Sleep(between);
        FlushBatchDraw();

        putimage(0, 0, &bk);
        putimage(0, 300, &bksh);
        IMAGE t15;
        loadimage(&t15, _T("t15.jpg"), 260, 240, true);
        IMAGE y15;
        loadimage(&y15, _T("y15.jpg"), 260, 240, true);
        putimage(-10, vertical + 4, 260, 240, &y15, 0, 0, SRCAND);
        putimage(-10, vertical + 4, 260, 240, &t15, 0, 0, SRCPAINT);
        Sleep(between);
        FlushBatchDraw();

        putimage(0, 0, &bk);
        putimage(0, 300, &bksh);
        IMAGE t16;
        loadimage(&t16, _T("t16.jpg"), 260, 240, true);
        IMAGE y16;
        loadimage(&y16, _T("y16.jpg"), 260, 240, true);
        putimage(-10, vertical + 4, 260, 240, &y16, 0, 0, SRCAND);
        putimage(-10, vertical + 4, 260, 240, &t16, 0, 0, SRCPAINT);
        Sleep(between);
        FlushBatchDraw();
        
        if (_kbhit())
        {
            key = _getch();
            putimage(182, 200, &bkim);
            switch (key)
            {
            case '1':
                sheep = 1;
                putimage(193, 208, 94, 94, &duiyanma, 0, 0, SRCAND);
                putimage(193, 208, 94, 94, &duitu, 0, 0, SRCPAINT);
                getimage(&bk, 0, 0, 640, 480);
                break;
            case '2':
                
                sheep = 2;
                putimage(306, 208, 94, 94, &duiyanma, 0, 0, SRCAND);
                putimage(306, 208, 94, 94, &duitu, 0, 0, SRCPAINT);
                getimage(&bk, 0, 0, 640, 480);
                break;
            case '3':
                
                sheep = 3;
                putimage(412, 208, 94, 94, &duiyanma, 0, 0, SRCAND);
                putimage(412, 208, 94, 94, &duitu, 0, 0, SRCPAINT);
                getimage(&bk, 0, 0, 640, 480);
                break;
            case '4':
                
                sheep = 4;
                putimage(523, 208, 94, 94, &duiyanma, 0, 0, SRCAND);
                putimage(523, 208, 94, 94, &duitu, 0, 0, SRCPAINT);
                getimage(&bk, 0, 0, 640, 480);
                break;
            case 'p':
            case 'P':
                key = 1;
                out = 1;
                break;
            default:
                out = 1;
                break;
            }
        }
        if (out == 1)
            break;
    }
    EndBatchDraw();
}

void content()
{
    IMAGE img;
    loadimage(&img, _T("bkimg2.jpg"), 640, 480, true);
    putimage(0, 0, &img);

    matrix[0][0] = 0;
    matrix[n - 1][n - 1] = 0;

    setfillcolor(LIGHTGRAY);
    solidrectangle(movex - over, movey - over, windowSize + movex + over, windowSize + movey + over);

    for (int row = 0; row < n; row++)
    {
        for (int col = 0; col < n; col++)
        {
            x = movex + spacings + col * (cellSize + spacing);
            y = movey + spacings + row * (cellSize + spacing);

            if (matrix[row][col] == 0)
                setfillcolor(LIGHTCYAN);
            else
                setfillcolor(BLUE);

            solidrectangle(x, y, x + cellSize, y + cellSize);
        }
    }

    IMAGE img2;
    loadimage(&img2, _T("house.jpg"), cellSize + 1, cellSize + 1, true);
    putimage(x, y, &img2);

    setlinecolor(RED);
    setlinestyle(PS_SOLID | PS_JOIN_MITER | PS_ENDCAP_SQUARE, linew);
    moveto(lines + movex, lines + movey);
    lineto(windowSize - lines + movex, lines + movey);
    lineto(windowSize - lines + movex, windowSize - lines + movey);
    lineto(lines + movex, windowSize - lines + movey);
    lineto(lines + movex, lines + movey);
}

void KeyDown()
{
    int c;
    if (_kbhit())
    {
    here:
        c = _getch();
        switch (c)
        {
        case 'a':
        case 'A':
        case 75:
            wayx = -1;
            wayy = 0;
            break;
        case 'd':
        case 'D':
        case 77:
            wayx = 1;
            wayy = 0;
            break;
        case 'w':
        case 'W':
        case 72:
            wayx = 0;
            wayy = -1;
            break;
        case 's':
        case 'S':
        case 80:
            wayx = 0;
            wayy = 1;
            break;
        case 224:
            goto here;
            break;
        default:
            wayx = 0;
            wayy = 0;
        }
    }
}

void move()
{
    setfillcolor(WHITE);
    setlinecolor(BLACK);
    fillrectangle(x1, yy1, x2, y2);

    setbkmode(TRANSPARENT);
    settextcolor(BLACK);
    settextstyle(y2 - yy1 - 5, 0, _T("Î¢ÈíÑÅºÚ"));
    RECT r1 = { x1, yy1, x2, y2 };
    drawtext(_T("Help"), &r1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    IMAGE map;
    getimage(&map, movex - over, movey - over, windowSize + movex + over, windowSize + movey + over);

    IMAGE map2;
    getimage(&map2, x5, y5, x6, y6);

    IMAGE map3;
    getimage(&map3, x7, y7, x8, y8);

    x = spacings + movex;
    y = spacings + movey;

    BeginBatchDraw();

    time_t tBegin = 0;
    time_t tEnd = 0;
    time_t tDiff = 0;
    time_t tDiffTmp = 0;
    time_t times = 0;
    int iHour = 0;
    int iMin = 0;
    int iSec = 0;
    tBegin = time(0);

    int i = 0, j = 0, k = 0;
    while (1)
    {
        there:
        if (_kbhit())
            k = 0;
        if (k == 0)
        {
            putimage(movex - over, movey - over, &map);

            if (hide == 0)
            {
                solidrectangle(x, y, x + cellSize, y + cellSize);

                IMAGE sheeps;
                switch (sheep)
                {
                case 1:
                    loadimage(&sheeps, _T("sheep1.jpg"), cellSize + 1, cellSize + 1, true);
                    break;
                case 2:
                    loadimage(&sheeps, _T("sheep2.jpg"), cellSize + 1, cellSize + 1, true);
                    break;
                case 3:
                    loadimage(&sheeps, _T("sheep3.jpg"), cellSize + 1, cellSize + 1, true);
                    break;
                case 4:
                    loadimage(&sheeps, _T("sheep4.jpg"), cellSize + 1, cellSize + 1, true);
                    break;
                }
                putimage(x, y, &sheeps);
            }

            if (_kbhit())
            {
                KeyDown();
                if (matrix[yy + wayy][xx + wayx] != 1 \
                    && xx + wayx >= 0 && xx + wayx < n \
                    && yy + wayy >= 0 && yy + wayy < n)
                {
                    xx += wayx;
                    yy += wayy;
                    steps--;
                    
                    for (int i = 0; i < 6; i++)
                    {
                        putimage(movex - over, movey - over, &map);
                        setfillcolor(WHITE);
                        solidrectangle(x, y, x + cellSize, y + cellSize);

                        IMAGE sheeps;
                        switch (sheep)
                        {
                        case 1:
                            loadimage(&sheeps, _T("sheep1.jpg"), cellSize + 1, cellSize + 1, true);
                            break;
                        case 2:
                            loadimage(&sheeps, _T("sheep2.jpg"), cellSize + 1, cellSize + 1, true);
                            break;
                        case 3:
                            loadimage(&sheeps, _T("sheep3.jpg"), cellSize + 1, cellSize + 1, true);
                            break;
                        case 4:
                            loadimage(&sheeps, _T("sheep4.jpg"), cellSize + 1, cellSize + 1, true);
                            break;
                        }
                        putimage(x, y, &sheeps);

                        if (animate == 1)
                            FlushBatchDraw();
                        x += (cellSize + spacing) / 6 * wayx;
                        y += (cellSize + spacing) / 6 * wayy;
                        Sleep(flash);
                    }
                }
            }

            if (MouseHit())
            {
                MOUSEMSG msg = GetMouseMsg();
                if (msg.uMsg == WM_LBUTTONDOWN)
                {
                    if (msg.x >= x1 && msg.x <= x2 \
                        && msg.y >= yy1 && msg.y <= y2)
                    {

                        BEGIN_X = yy;
                        BEGIN_Y = xx;
                        value();

                        settextstyle(y2 - yy1 - 8, 0, _T("Î¢ÈíÑÅºÚ"));
                        RECT r1 = { x3, y3, x4, y4 };
                        FlushBatchDraw();
                        k = 1;
                    }
                }
            }
        }
        putimage(x5, y5, &map2);
        putimage(x7, y7, &map3);
        Sleep(delay);

        tEnd = time(0);
        tDiff = difftime(tEnd, tBegin);
        timeuse = tDiff;
        times = difftime(maxtime, tDiff);
        iHour = times / 3600;
        tDiffTmp = times % 3600;
        iMin = times / 60;
        iSec = times % 60;
        settextcolor(BLUE);
        settextstyle(20, 0, _T("·ÂËÎ"));
        TCHAR strtime[9];
        TCHAR strstep[9];

        RECT r4 = { x5, y5 - 25, x6 + 20, y6 - 25 };
        drawtext(_T("Time £º"), &r4, DT_CENTER | DT_VCENTER);

        _sntprintf_s(strtime, sizeof(strtime), _T("%02d:%02d:%02d"), iHour, iMin, iSec);
        outtextxy(x5, y5, strtime);

        RECT r5 = { x7, y7 - 25, x8 + 20, y8 - 25 };
        drawtext(_T("Steps £º"), &r5, DT_CENTER | DT_VCENTER);

        _sntprintf_s(strstep, sizeof(strstep), _T("%d"), steps);
        outtextxy(x7 + 30, y7, strstep);
        FlushBatchDraw();

        if ((xx == n - 1 && yy == n - 1 || steps == 0  || timeuse == maxtime) && j == 0)
        {
            i = 1;
            j = 1;
        }
        if (i > 0)
            i++;
        if (i >= 3)
        {
            EndBatchDraw();
            break;
        }
    }
}

void endpage()
{
    setbkcolor(WHITE);
    cleardevice();
    settextcolor(BLUE);

    IMAGE img;
    loadimage(&img, _T("bkimg3.jpg"), 640, 480, true);
    putimage(0, 0, &img);
    setbkmode(TRANSPARENT);

    int mark;
    if (steps == 0 || timeuse == maxtime)
        mark = 0;
    else
        mark = steps * 100 - timeuse;

    TCHAR marks[20];
    settextstyle(25, 0, _T("Algerian"));
    _sntprintf_s(marks, sizeof(marks), _T("Marks : %d"), mark);
    outtextxy(255, 300, marks);
}

int main()
{
    initgraph(640, 480);
    title();
    if (key == 1)
        return 0;
    content();
    move();
    Sleep(300);
    endpage();
    _getch();
    return 0;
}