#include <iostream>
#include <cstring>
#include <string>
#include "include/cinatra.hpp"
#include "include/rapidjson.hpp"
#include "interface.h"
#include "datastruct.h"

using namespace std;
using namespace cinatra;
using namespace rapidjson;

int main() {
	http_server server(std::thread::hardware_concurrency());
    server.listen("127.0.0.1", "8888");
    mapInit();

	server.set_http_handler<GET, POST>("/", [](request &req, response &res) {
        isReady = 0;
        isRunning = 0;
        timeTot = 0.0;
        timeInit();

        res.redirect("/index.html");
    });

	server.set_http_handler<GET, POST>("/api/getmap", [](request &req, response &res){
        isReady = 0;
        isRunning = 0;
        timeTot = 0.0;
        timeInit();

        char *retStr;
        getNode(retStr);
		res.set_status_and_content(status_type::ok, retStr, req_content_type::json);
        delete[] retStr;
    });

    server.set_http_handler<GET, POST>("/api/getedge", [](request &req, response &res){
        isReady = 0;
        isRunning = 0;
        timeTot = 0.0;
        timeInit();

        char *retStr;
        getEdge(retStr);
		res.set_status_and_content(status_type::ok, retStr, req_content_type::json);
        delete[] retStr;
    });

    server.set_http_handler<GET, POST>("/api/newperson", [](request &req, response &res){
        cout << "/api/newperson start " << endl;
        isReady = 1;
        isRunning = 0;
        timeTot = 0.0;
        timeInit();

        auto pStrategy = stoi(string(req.get_query_value("pStrategy")));
        auto pType = stoi(string(req.get_query_value("pType")));
        auto pName = string(req.get_query_value("pName"));
        auto pStCamp = stoi(string(req.get_query_value("pStCamp")));
        auto pStId = stoi(string(req.get_query_value("pStId")));
        auto pEnCamp = stoi(string(req.get_query_value("pEnCamp")));
        auto pEnId = stoi(string(req.get_query_value("pEnId")));
        auto pStartTime = stoi(string(req.get_query_value("pStartTime")));

        if (pName.empty()) {
            res.set_status_and_content(status_type::bad_request, "Name Empty");
            return;
        }

        auto isChange = stoi(string(req.get_query_value("isChange")));
        double pNowX, pNowY;
        if (isChange == 1) {
            pNowX = stod(string(req.get_query_value("pNowX")));
            pNowY = stod(string(req.get_query_value("pNowY")));
            newTempNode(pNowX, pNowY);
            assert(pStCamp == bkpCamp);
            pStCamp = bkpCamp;
            pStId = nNode - 1;
        }

        auto isTravel = stoi(string(req.get_query_value("isTravel")));
        if (isTravel == 0) {
            int timeNow = (time(NULL) + 8 * 3600) % 86400;
            printTime(timeNow);
            logout << "Begin planning: " << pTable[pStCamp][pStId].nName << "->" << pTable[pEnCamp][pEnId].nName << endl;

            char *retStr;
            newPerson(retStr, pStrategy, pType, pName, pStCamp, pStId, pEnCamp, pEnId, timeNow);
            res.set_status_and_content(status_type::ok, retStr, req_content_type::json);
            delete[] retStr;

            printTime(timeNow);
            logout << "Begin planning: " << pTable[pStCamp][pStId].nName << "->" << pTable[pEnCamp][pEnId].nName << endl;
        } else if (isTravel == 1) {
            int cnt = stoi(string(req.get_query_value("cnt")));
            cout << "cnt = " << cnt << endl;
            cout << "body: " << req.body() << endl;

            if (cnt < 2) {
                res.set_status_and_content(status_type::bad_request, "cnt too less!!");
                return;
            } else {
                cout << "isTravel = 1 start" << endl;
            }

            vector<Node> nodeList;
            nodeList.clear();
            for (int i = 0; i < cnt; ++i) {
                string pNodeJson = "pNode%5B" + to_string(i);
                pNodeJson = pNodeJson + "%5D%5BpCamp%5D";
                int pNode = stoi(string(req.get_query_value(pNodeJson)));

                string pIdJson = "pNode%5B" + to_string(i);
                pIdJson = pIdJson + "%5D%5BpId%5D";
                int pId = stoi(string(req.get_query_value(pIdJson)));

                nodeList.push_back(Node(NOTEXIST, pNode, pId, "No see"));
                cout << "size = " << nodeList.size() << ", node = " << pNode << ", id = " << pId << endl;
            }
            
            pStCamp = nodeList[0].nCampus;
            pStId = nodeList[0].nId;
            pEnCamp = nodeList.back().nCampus;
            pEnId = nodeList.back().nId;

            int timeNow = (time(NULL) + 8 * 3600) % 86400;
            printTime(timeNow);
            logout << "Begin planning: " << pTable[pStCamp][pStId].nName << "->" << pTable[pEnCamp][pEnId].nName << endl;

            char *retStr;
            newTravel(retStr, pStrategy, pType, nodeList, timeNow);
            res.set_status_and_content(status_type::ok, retStr, req_content_type::json);
            delete[] retStr;

            printTime(timeNow);
            logout << "Begin planning: " << pTable[pStCamp][pStId].nName << "->" << pTable[pEnCamp][pEnId].nName << endl;
        }
        if (isChange == 1)
            renewTempNode();
    });

    server.set_http_handler<GET, POST>("/api/start", [](request &req, response &res) {
        if (isReady == 0) {
            res.set_status_and_content(status_type::bad_request, "ready defeat");
            cout << "not ready but start!!!" << endl;
            return;
        }
        isRunning = 1;
        timeTot = 0.0;
        timeInit();

        char *retStr;
        updatePoint(retStr);
        res.set_status_and_content(status_type::ok, retStr, req_content_type::json);
        delete[] retStr;
    });

    server.set_http_handler<GET, POST>("/api/update", [](request& req, response& res) {
        if (isRunning == 0) {
            res.set_status_and_content(status_type::bad_request, "ready defeat");
            cout << "No running but update!" << endl;
            return;
        }
        char *retStr;
        updatePoint(retStr);
        res.set_status_and_content(status_type::ok, retStr, req_content_type::json);
        delete[] retStr;
    });

    server.set_http_handler<GET, POST>("/api/pause", [](request &req, response &res){
        isRunning = 0;
        char *retStr;
        updatePoint(retStr, 1);
        res.set_status_and_content(status_type::ok, retStr, req_content_type::json);
        delete[] retStr;
        cout << "pause ok" << endl;
    });

    server.set_http_handler<GET, POST>("/api/continue", [](request& req, response& res) {
        timeInit();
        isRunning = 1;
        char *retStr;
        updatePoint(retStr);
        res.set_status_and_content(status_type::ok, retStr, req_content_type::json);
        delete[] retStr;
        cout << "continue ok" << endl;
    });

    server.set_http_handler<GET, POST>("/api/stop", [](request& req, response& res) {
        isReady = 0;
        isRunning = 0;
        timeTot = 0.0;
        timeInit();
        
        char *retStr;
        updatePoint(retStr);
        res.set_status_and_content(status_type::ok, retStr, req_content_type::json);
        delete[] retStr;
        cout << "Stopped!" << endl;
    });

    server.set_http_handler<GET, POST>("/api/change", [](request &req, response &res) {
        isReady = 0;
        isRunning = 0;
        timeTot = 0.0;
        timeInit();

        char *retStr;
        updatePoint(retStr);
        res.set_status_and_content(status_type::ok, retStr, req_content_type::json);
        delete[] retStr;
        cout << "Have been changed!" << endl;
    });

    server.set_http_handler<GET, POST>("/api/getnearby", [](request& req, response& res) {
        auto camp = stoi(string(req.get_query_value("camp")));
        auto x = stod(string(req.get_query_value("x")));
        auto y = stod(string(req.get_query_value("y")));

        char *retStr;
        queryNearby(retStr, camp, x, y);
        res.set_status_and_content(status_type::ok, retStr, req_content_type::json);
        delete[] retStr;
    });

    server.set_http_handler<GET, POST>("/api/multiset", [](request &req, response &res){
        char *retStr;
        updatePoint(retStr);
        res.set_status_and_content(status_type::ok, retStr, req_content_type::json);
        delete[] retStr;

        MULTISPEED = (int)stoi(string(req.get_query_value("cnt")));
        MULTISPEED = max(MULTISPEED, 1);
        MULTISPEED = min(MULTISPEED, 1000);
        cout << "Now MULTISPEED = " << MULTISPEED << endl;
    });

    server.run();
    return 0;
}