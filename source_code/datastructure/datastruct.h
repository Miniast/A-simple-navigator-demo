#ifndef DATASTRUCT_H
#define DATASTRUCT_H

#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include <queue>

constexpr int DOOR0 = 15; /* SHAHE */
constexpr int DOOR1 = 65; /* XITUCHENG */
constexpr double SPEEDFOOT = 1.0;
constexpr double SPEEDBICYCLE = 4.0;
constexpr double SPEEDBUS = 12.0;
constexpr double SPEEDSCHOOLBUS = 14.0;

struct PointTable {
    double x, y;
    std::string nName;
    PointTable() {}
    PointTable(double x, double y, std::string nName) :
        x(x), y(y), nName(nName) {}
};

#define NOTEXIST 0
#define EXIST 1
#define SERVICE 2

struct Node {
    int nType;
    int nCampus, nId;
    std::string nName;
    Node() {}
    Node(int nType, int nCampus, int nId, std::string nName) :
        nType(nType), nCampus(nCampus), nId(nId), nName(nName) {}
};

#define FOOT 0
#define BICYCLE 1
#define BUS 2

struct Edge {
    int eType;
    int eCampus;
    int eStart, eEnd;
    double eCrowd;
    double dis;
    Edge() {}
    Edge(int eType, int eCampus, int eStart, int eEnd, double eCrowd, double dis) :
        eType(eType), eCampus(eCampus), eStart(eStart), eEnd(eEnd), eCrowd(eCrowd), dis(dis) {}
};

struct Line {
    int lType;
    int lStCamp, lStId;
    int lEnCamp, lEnId;
    double nowTime; /* 起始时间为0，已经经过的秒数 */
    double nowDis; /* 起始距离为0，已经经过的米数 */
    /* 以上两个参数均表示到达终止节点的秒数和米数 */
    Line *lNext;
    Line() :
        lType(0), lStCamp(0), lStId(0), lEnCamp(0), lEnId(0), nowTime(0.0), nowDis(0.0), lNext(NULL) {}
};

constexpr int StrategyLIMIT = 2;
#define DISTFIRST 0
#define TIMEFIRST 1

struct Person {
    int pStrategy;
    int pType;
    std::string pName;
    int pStCamp, pStId;
    int pEnCamp, pEnId;
    double pStartTime;
    int pLen;
    Line *phead;
    Person() {}
    Person(int pStrategy, int pType, std::string pName, int pStCamp, int pStId, int pEnCamp, int pEnId, double pStartTime) :
        pStrategy(pStrategy), pType(pType), pName(pName), pStCamp(pStCamp), pStId(pStId),
        pEnCamp(pEnCamp), pEnId(pEnId), pStartTime(pStartTime), pLen(0), phead(NULL) {}
};

#define SCHOOLBUS 0
#define GENERALBUS 1

struct Bus {
    int bType;
    int bStCamp, bStId;
    int bEnCamp, bEnId;
    double bStartTime;
    double dis;
    Bus() {}
    Bus(int bType, int bStCamp, int bStId, int bEnCamp, int bEnId, double bStartTime, double dis) :
        bType(bType), bStCamp(bStCamp), bStId(bStId), bEnCamp(bEnCamp), bEnId(bEnId), bStartTime(bStartTime), dis(dis) {}
};

extern int nNode, nEdge;
extern std::vector<Node> pNode;
extern std::vector<std::vector<PointTable>> pTable;
extern std::vector<std::vector<Edge>> mymap;
extern std::vector<Bus> busTable;
extern void dijkstra(Person *person);

#endif // DATASTRUCT_H