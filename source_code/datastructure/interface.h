#ifndef INTERFACE_H
#define INTERFACE_H

#include <iostream>
#include <cstring>
#include <string>
#include <ctime>
#include <sys/timeb.h>
#include <sys/types.h>
#include "include/rapidjson.hpp"
#include "include/cinatra.hpp"
#include "datastruct.h"

#define MAPFILE "./data/map.txt"
#define BUSFILE "./data/bus.txt"

constexpr double DISRATE[] = {0.45, 8.0/11.0};
constexpr double DISBETWEEN = 29000.0;
constexpr double NEARLYDIS = 300;

extern int MULTISPEED;
extern bool isReady;
extern bool isRunning;
extern double timeTot;
extern int bkpCamp, bkpSt, bkpEn;
extern double logTime;
extern std::ofstream logout;

extern void printTime(int timeAdd);
extern void timeInit();
extern unsigned int timeDelta();
extern void mapInit();

extern void newTempNode(double pNowX, double pNowY);
extern void renewTempNode();
extern void getNode(char *&retStr);
extern void getEdge(char *&retStr);
extern void newPerson(char *&retStr, int pStrategy, int pType, std::string pName, int pStCamp, int pStId, int pEnCamp, int pEnId, int pStartTime);
extern void newTravel(char *&retStr, int pStrategy, int pType, std::vector<Node> &nodeList, int pStartTime);
extern void updatePoint(char *&retStr, bool isPause = 0);
void queryNearby(char *&retStr, int camp, double x, double y);

#endif // INTERFACE_H