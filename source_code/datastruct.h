#ifndef DATASTRUCT_H
#define DATASTRUCT_H

#include <iostream>
#include <cstring>
#include <algorithm>
#include <string>
#include <vector>

constexpr int DOOR0 = 10; /* SHAHE */
constexpr int DOOR1 = 60; /* XITUCHENG */
constexpr double SPEEDFOOT = 1.0;
constexpr double SPEEDBICYCLE = 4.0;
constexpr double SPEEDBUS = 12.0;

struct PointTable {
    double x, y;
    std::string nName;
    PointTable() {}
    PointTable(double x, double y, std::string nName) :
        x(x), y(y), nName(nName) {}
};

#define NOTEXIST 0
#define EXIST 1
#define DOOR 2

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
    double nowTime;
    Line *lNext;
};

constexpr int StrategyLIMIT = 2;
#define DISTFIRST 0
#define TIMEFIRST 1

struct Person {
    int pStrategy;
    int pType;
    /* int pCampus, pId; */
    std::string pName;
    int pStCamp, pStId;
    int pEnCamp, pEnId;
    double pStartTime;
    int pLen;
    Line *phead;
    Person() {}
    Person(int pStrategy, int pType, std::string pName, int pStCamp, int pStId, int pEnCamp, int pEnId, int pStartTime) :
        pStrategy(pStrategy), pType(pType), pName(pName), pStCamp(pStCamp), pStId(pStId), pEnCamp(pEnCamp),
        pEnId(pEnId), pStartTime(pStartTime), phead(NULL) {}
};

struct Bus {
    int bStCamp, bStId;
    int bEnCamp, bEnId;
    double bStartTime;
    double dis;
    Bus() {}
    Bus(int bStCamp, int bStId, int bEnCamp, int bEnId, double bStartTime, double dis) :
        bStCamp(bStCamp), bStId(bStId), bEnCamp(bEnCamp), bEnId(bEnId), bStartTime(bStartTime), dis(dis) {}
};

extern int nNode, nEdge;
extern std::vector<Node> pNode;
extern std::vector<std::vector<PointTable>> pTable;
extern std::vector<std::vector<Edge>> mymap;
extern std::vector<Bus> busTable;

extern void dijkstra(Person *person);

#endif // DATASTRUCT_H