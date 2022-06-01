#include <bits/stdc++.h>
#include "datastruct.h"

using namespace std;

#define MAPFILE "./data/map.txt"
#define BUSFILE "./data/bus.txt"

double distance(double x,double y)
{
	return(sqrt(x*x+y*y));
}

map<int, int> corrMap;

void mapInit() {
    ifstream mapIn(MAPFILE, ios::in);
    mapIn >> nNode >> nEdge;
    cout << nNode << " " << nEdge << endl;
    corrMap.clear();
    for (int i = 0, en = nNode + 10; i <= en; ++i)
        corrMap[i] = i;

    pTable.resize(2);
    pTable[0].resize(nNode);
    pTable[1].resize(nNode);
    int nType, nCampus, nId;
    double x, y;
    string nName;

    for (int i = 0; i < nNode; ++i) {
        /* 校区 编号 x y 名字 */
        mapIn >> nType >> nCampus >> nId >> x >> y >> nName;
        pNode.push_back(Node(nType, nCampus, nId, nName));
        pTable[nCampus][nId] = PointTable(x, y, nName);
    }

    mymap.resize(nNode<<1);
    int eType, eCampus, eStart, eEnd;
    double eCrowd, dis;

    for (int i = 0; i < nEdge; ++i) {
        /* 校区 编号 x y 名字 */
        mapIn >> eType >> eCampus >> eStart >> eEnd >> eCrowd;
        dis = 0.45 * distance(pTable[eCampus][eStart].x - pTable[eCampus][eEnd].x, pTable[eCampus][eStart].y - pTable[eCampus][eEnd].y);
        /* dis 单位 m */
        mymap[eStart].push_back(Edge(eType, eCampus, eStart, eEnd, eCrowd, dis));
        mymap[eEnd].push_back(Edge(eType, eCampus, eEnd, eStart, eCrowd, dis));
    }
    
    int corrCnt; mapIn >> corrCnt;
    for (int i = 0; i < corrCnt; ++i) {
        int xCampus, xId, yCampus, yId;
        mapIn >> xCampus >> xId >> yCampus >> yId;
        corrMap[xId] = yId;
    }
    mapIn.close();

    ifstream busIn(BUSFILE, ios::in);
    int bType;
    int bStCamp, bStId;
    int bEnCamp, bEnId;
    double bStartTime;
    
    int busCnt;
    busIn >> busCnt;
    for (int i = 0; i < busCnt; ++i) {
        busIn >> bType >> bStCamp >> bStId >> bEnCamp >> bEnId >> bStartTime;
        busTable.push_back(Bus(bType, bStCamp, bStId, bEnCamp, bEnId, bStartTime, 29000));
    }

    printf("Init Sucessfully!\n");
}

void newPerson() {
    Person person(0, 0, "test", 0, 6, 1, 81, 52609);
    dijkstra(&person);
    cout << "person.pLen = " << person.pLen << endl;
    for (Line *ptr = person.phead; ptr != NULL; ptr = ptr->lNext) {
        cout << "ptr->lEnCamp = " << ptr->lEnCamp << ", ptr->lEnId = " << ptr->lEnId
            << ", ptr->nowTime = "<< ptr->nowTime << ", ptr->nowDis = " << ptr->nowDis;
    }
}

int main() {
    cout << (int)time(NULL) % 86400 << endl;
	mapInit();
	newPerson();
}