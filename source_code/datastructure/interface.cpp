#include "interface.h"

using namespace std;
using namespace rapidjson;

int MULTISPEED = 20;
bool isReady;
bool isRunning;
double timeTot;
int bkpCamp, bkpSt, bkpEn;
double logTime;
ofstream logout;

struct _timeb epoch;
Line *pHead, *linePos, *linePre;
vector<Edge> edgeList;
map<int, int> corrMap;

inline static double distance(double x, double y) {
    return sqrt(x * x + y * y);
}

void printTime(int timeAdd) {
    time_t _times = time(NULL); 
    tm *t = localtime(&_times);
    int hours = timeAdd / 3600;
    timeAdd %= 3600;
    int minutes = timeAdd / 60;
    timeAdd %= 60;
    int seconds = timeAdd;

    char buffer[500];
    sprintf(buffer, "%d/%02d/%02d %02d:%02d:%02d ",
        t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, hours, minutes, seconds);
    logout << buffer;
}

void timeInit() {
    _ftime(&epoch);
	// printf("Now time: %d: %d\n", epoch.time, epoch.millitm);
}

unsigned int timeDelta() {
    struct _timeb timeNow;
	_ftime(&timeNow);
    unsigned int delta = (unsigned int)((timeNow.time - epoch.time) * 1000 + (timeNow.millitm - epoch.millitm));
	// printf("Now time: %d: %d\n", timeNow.time, timeNow.millitm);
    // printf("And the time delta is %u\n", delta);
    return delta;
}

void mapInit() {
    logout.close();
    logout.open("./log.txt",ios::app);
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
        dis = DISRATE[eCampus] * distance(pTable[eCampus][eStart].x - pTable[eCampus][eEnd].x, pTable[eCampus][eStart].y - pTable[eCampus][eEnd].y);
        /* dis 单位 m */
        mymap[eStart].push_back(Edge(eType, eCampus, eStart, eEnd, eCrowd, dis));
        mymap[eEnd].push_back(Edge(eType, eCampus, eEnd, eStart, eCrowd, dis));
        edgeList.push_back(Edge(eType, eCampus, eStart, eEnd, eCrowd, dis));
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
        busTable.push_back(Bus(bType, bStCamp, bStId, bEnCamp, bEnId, bStartTime, DISBETWEEN));
    }

    printf("Init Sucessfully!\n");
}

void newTempNode(double pNowX, double pNowY) {
    pNode.push_back(Node(NOTEXIST, bkpCamp, nNode, "Temp Place"));
    pTable[0].push_back(PointTable(pNowX, pNowY, "Temp Place"));
    pTable[1].push_back(PointTable(pNowX, pNowY, "Temp Place"));

    double disSt = DISRATE[bkpCamp] * distance(pTable[bkpCamp][bkpSt].x - pNowX, pTable[bkpCamp][bkpSt].y - pNowY);
    mymap[nNode].push_back(Edge(BICYCLE, bkpCamp, nNode, bkpSt, 0.0, disSt));
    mymap[bkpSt].push_back(Edge(BICYCLE, bkpCamp, bkpSt, nNode, 0.0, disSt));

    double disEn = DISRATE[bkpCamp] * distance(pTable[bkpCamp][bkpEn].x - pNowX, pTable[bkpCamp][bkpEn].y - pNowY);
    mymap[nNode].push_back(Edge(BICYCLE, bkpCamp, nNode, bkpEn, 0.0, disEn));
    mymap[bkpEn].push_back(Edge(BICYCLE, bkpCamp, bkpEn, nNode, 0.0, disEn));
    ++nNode;
}

void renewTempNode() {
    --nNode;
    pNode.pop_back();
    pTable[0].pop_back();
    pTable[1].pop_back();
    mymap[nNode].pop_back();
    mymap[bkpSt].pop_back();
    mymap[nNode].pop_back();
    mymap[bkpEn].pop_back();
}

void getNode(char *&retStr) {
    Document resultJson;
    Document::AllocatorType &allocator = resultJson.GetAllocator();

    Value jsPoint(kArrayType), tmp, jsName;
    for (int i = 0; i < nNode; ++i) {
        if (pNode[i].nType == 0) // NOTEXIST
            continue;
        jsName.SetString(pNode[i].nName.c_str(), pNode[i].nName.size(), allocator);
        tmp.SetObject()
            .AddMember("nCampus", pNode[i].nCampus, allocator)
            .AddMember("nId", pNode[i].nId, allocator)
            .AddMember("nName", jsName, allocator)
            .AddMember("x", pTable[pNode[i].nCampus][pNode[i].nId].x, allocator)
            .AddMember("y", pTable[pNode[i].nCampus][pNode[i].nId].y, allocator);
        jsPoint.PushBack(tmp, allocator);
    }
    resultJson.SetObject().AddMember("nPoint", jsPoint, allocator);

    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    resultJson.Accept(writer);
    string jsStr = buffer.GetString();
    retStr = new char[jsStr.length()];
    strcpy(retStr, jsStr.c_str());
}

void getEdge(char *&retStr) {
    Document resultJson;
    Document::AllocatorType &allocator = resultJson.GetAllocator();

    Value jsEdge(kArrayType), tmp;
    for (int i = 0; i < nEdge; ++i) {
        tmp.SetObject()
            .AddMember("eType", edgeList[i].eType, allocator)
            .AddMember("eCampus", edgeList[i].eCampus, allocator)
            .AddMember("eStart", edgeList[i].eStart, allocator)
            .AddMember("eEnd", edgeList[i].eEnd, allocator)
            .AddMember("eCrowd", edgeList[i].eCrowd, allocator);
        jsEdge.PushBack(tmp, allocator);
    }
    resultJson.SetObject()
                .AddMember("eEdge", jsEdge, allocator);

    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    resultJson.Accept(writer);
    string jsStr = buffer.GetString();
    retStr = new char[jsStr.length()];
    strcpy(retStr, jsStr.c_str());
}

void newPerson(char *&retStr, int pStrategy, int pType, string pName, int pStCamp, int pStId, int pEnCamp, int pEnId, int pStartTime) {
    pStId = corrMap[pStId];
    pEnId = corrMap[pEnId];
    cout << "newPerson() start" << endl;
    Person person(pStrategy, pType, pName, pStCamp, pStId, pEnCamp, pEnId, pStartTime);
    cout << pStrategy << " " << pType << " " << pName << " " << pStCamp << " "<< pStId << " " << pEnCamp << " " << pEnId << " " << pStartTime << endl;
    dijkstra(&person);

    pHead = person.phead;
    linePos = person.phead;
    linePre = NULL;

    Document resultJson;
    Document::AllocatorType &allocator = resultJson.GetAllocator();
    
    Value jsName, jsPoint(kArrayType), tmp;
    double pTime, pLength;

    jsName.SetString(person.pName.c_str(), person.pName.size(), allocator);
    if (person.pLen > 0) {
        ++person.pLen;
        tmp.SetObject()
            .AddMember("camp", person.phead->lStCamp, allocator)
            .AddMember("id", person.phead->lStId, allocator);
        jsPoint.PushBack(tmp, allocator);
    }
    for (Line *it = person.phead; it != NULL; it = it->lNext) {
        cout << it->lEnCamp << " " << it->lEnId << endl;
        tmp.SetObject()
            .AddMember("camp", it->lEnCamp, allocator)
            .AddMember("id", it->lEnId, allocator);
        jsPoint.PushBack(tmp, allocator);
        pTime = it->nowTime;
        pLength = it->nowDis;
    }

    resultJson.SetObject()
                .AddMember("pName", jsName, allocator)
                .AddMember("pStartTime", person.pStartTime, allocator)
                .AddMember("pLen", person.pLen, allocator)
                .AddMember("point", jsPoint, allocator)
                .AddMember("pTime", pTime, allocator)
                .AddMember("pLength", pLength, allocator);

    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    resultJson.Accept(writer);
    string jsStr = buffer.GetString();
    cout << "Newperson out.json:" << endl;
    cout << jsStr << endl;
    retStr = new char[jsStr.length()];
    strcpy(retStr, jsStr.c_str());
}

void newTravel(char *&retStr, int pStrategy, int pType, vector<Node> &nodeList, int pStartTime) {
    cout << "newTravel start" << endl;
    for(auto &i: nodeList) {
        i.nId = corrMap[i.nId];
        cout << "camp = " << i.nCampus << ", id = " << i.nId << endl;
    }
    
    Person person(pStrategy, pType, nodeList[0].nName, nodeList[0].nCampus, nodeList[0].nId, nodeList[1].nCampus, nodeList[1].nId, pStartTime);
    dijkstra(&person);
    pHead = person.phead;
    linePos = person.phead;
    linePre = NULL;

    Line *pre;
    for (Line *it = person.phead; it != NULL; it = it->lNext)
        if (it->lNext == NULL) {
            pre = it;
            break;
        }

    Document resultJson;
    Document::AllocatorType &allocator = resultJson.GetAllocator();
    
    Value jsName, jsPoint(kArrayType), tmp;
    double pTime, pLength;
    jsName.SetString(person.pName.c_str(), person.pName.size(), allocator);
    int pStartTimeRet = person.pStartTime;
    int pLenTotal = 1;

    tmp.SetObject()
        .AddMember("camp", person.phead->lStCamp, allocator)
        .AddMember("id", person.phead->lStId, allocator);
    cout << "line camp = " << person.phead->lStCamp << ", line id = " << person.phead->lStId << endl;
    jsPoint.PushBack(tmp, allocator);

    for (int i = 1, en = (int)nodeList.size() - 1; i < en; ++i) {
        person = Person(pStrategy, pType, nodeList[i].nName, nodeList[i].nCampus, nodeList[i].nId, nodeList[i+1].nCampus, nodeList[i+1].nId, pStartTime + pre->nowTime);
        dijkstra(&person);
        pre->lNext = person.phead;
        pLenTotal += person.pLen;
        for (Line* it = person.phead; it != NULL; it = it->lNext) {
            it->nowTime += pre->nowTime;
            it->nowDis += pre->nowDis;
            if (it->lNext == NULL) {
                pre = it;
                break;
            }
        }
    }

    for (Line *it = pHead; it != NULL; it = it->lNext) {
        cout << "line camp = " << it->lEnCamp << ", line id = " << it->lEnId << endl;
        tmp.SetObject()
            .AddMember("camp", it->lEnCamp, allocator)
            .AddMember("id", it->lEnId, allocator);
        jsPoint.PushBack(tmp, allocator);
        pTime = it->nowTime;
        pLength = it->nowDis;
    }

    resultJson.SetObject()
                .AddMember("pName", jsName, allocator)
                .AddMember("pStartTime", pStartTimeRet, allocator)
                .AddMember("pLen", pLenTotal, allocator)
                .AddMember("point", jsPoint, allocator)
                .AddMember("pTime", pTime, allocator)
                .AddMember("pLength", pLength, allocator);

    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    resultJson.Accept(writer);
    string jsStr = buffer.GetString();
    cout << "Newtravel out.json:" << endl;
    cout << jsStr << endl;
    retStr = new char[jsStr.length()];
    strcpy(retStr, jsStr.c_str());
}

static void destory() {
    while (pHead != NULL) {
        Line *nxt = pHead->lNext;
        delete pHead;
        pHead = nxt;
    }
    cout << "destory() ok" << endl;
}

void updatePoint(char *&retStr, bool isPause) {
    timeTot += timeDelta() * MULTISPEED / 1000.0;
    logTime += timeDelta() * MULTISPEED / 1000.0;
    cout << "timedelta: " << timeDelta() << endl;
    timeInit();

    int jType, jCamp;
    double x, y;
    while (linePos && timeTot > linePos->nowTime) {
        linePre = linePos;
        linePos = linePos->lNext;
        if (linePos == NULL)
            break;
        cout << "linePos = " << (long long)linePos << ", time = " << linePos->nowTime << endl;
    }
    if (linePos == NULL) {
        isReady = 0;

        jType = 0;
        jCamp = 0;
        x = 0.0; y = 0.0;
        destory();
        if (isPause) {
            cout << "Aready destory but pause!!" << endl;
            assert(0);
        }
    } else if (linePos->lStCamp != linePos->lEnCamp) {
        jType = 3;
        jCamp = linePos->lEnCamp;
        x = 0.0; y = 0.0;
        if (isPause) {
            cout << "Between two campus!!" << endl;
            assert(0);
        }
    } else {
        jType = linePos->lType + 1;
        jCamp = linePos->lStCamp;
        double t = linePre == NULL ? 0 : linePre->nowTime;
        double rate = (timeTot - t) / (linePos->nowTime - t);
        x = pTable[linePos->lStCamp][linePos->lStId].x + (pTable[linePos->lEnCamp][linePos->lEnId].x - pTable[linePos->lStCamp][linePos->lStId].x) * rate;
        y = pTable[linePos->lStCamp][linePos->lStId].y + (pTable[linePos->lEnCamp][linePos->lEnId].y - pTable[linePos->lStCamp][linePos->lStId].y) * rate;
        cout << "Start Point Time = " << t << ", End Point Time = " << linePos->nowTime << ", Now Time = " << timeTot << endl;
        cout << "linePos->lStCamp = " << linePos->lStCamp << ", linePos->lStId = " << linePos->lStId << ", (x,y) = (" << x << ", " << y << "), Now rate = " << rate << endl;
        if (isPause) {
            bkpCamp = linePos->lStCamp;
            bkpSt = linePos->lStId;
            bkpEn = linePos->lEnId;
        }
    }

    Document resultJson;
    Document::AllocatorType &allocator = resultJson.GetAllocator();

    Value jsPoint(kArrayType);
    jsPoint.PushBack(x, allocator).PushBack(y, allocator);
    resultJson.SetObject()
                .AddMember("type", jType, allocator)
                .AddMember("camp", jCamp, allocator)
                .AddMember("point", jsPoint, allocator);
    
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    resultJson.Accept(writer);
    string jsStr = buffer.GetString();
    retStr = new char[jsStr.length()];
    strcpy(retStr, jsStr.c_str());
}

void queryNearby(char *&retStr, int camp, double x, double y) {
    Document resultJson;
    Document::AllocatorType &allocator = resultJson.GetAllocator();

    int cnt = 0;
    Value jsPoint(kArrayType), tmp, jsName;
    for (int i = 0; i < nNode; ++i) {
        if (camp != pNode[i].nCampus)
            continue;
        if (distance(x - pTable[pNode[i].nCampus][pNode[i].nId].x, y - pTable[pNode[i].nCampus][pNode[i].nId].y) > NEARLYDIS)
            continue;
        if (pNode[i].nType != 2)
            continue;
        ++cnt;
        jsName.SetString(pNode[i].nName.c_str(), pNode[i].nName.size(), allocator);
        tmp.SetObject()
            .AddMember("nType", pNode[i].nType, allocator)
            .AddMember("nCampus", pNode[i].nCampus, allocator)
            .AddMember("nId", pNode[i].nId, allocator)
            .AddMember("nName", jsName, allocator)
            .AddMember("x", pTable[pNode[i].nCampus][pNode[i].nId].x, allocator)
            .AddMember("y", pTable[pNode[i].nCampus][pNode[i].nId].y, allocator);
        jsPoint.PushBack(tmp, allocator);
    }
    resultJson.SetObject()
                .AddMember("cnt", cnt, allocator)
                .AddMember("nPoint", jsPoint, allocator);
    
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    resultJson.Accept(writer);
    string jsStr = buffer.GetString();
    /* cout << "queryNearby: " << jsStr << endl; */
    retStr = new char[jsStr.length()];
    strcpy(retStr, jsStr.c_str());
}