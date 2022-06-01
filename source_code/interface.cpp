#include "navigation.h"
#include "interface.h"

using namespace std;
using namespace cinatra;

void serverRun() {
    http_server server(std::thread::hardware_concurrency());
    server.listen("127.0.0.1", "8888");

	server.set_http_handler<GET, POST>("/", [](request &req, response &res) {
        res.redirect("/index.html");
    });
    mapInit();

	server.set_http_handler<GET, POST>("/api/getmap", [](request &req, response &res){
        char *retStr;
        getMap(retStr);
		res.set_status_and_content(status_type::ok, retStr, req_content_type::json);
        delete[] retStr;
    });

    server.set_http_handler<GET, POST>("/api/getbus", [](request &req, response &res){
        char *retStr;
        // getBusTable(retStr);
		res.set_status_and_content(status_type::ok, retStr, req_content_type::json);
        delete[] retStr;
    });

    server.set_http_handler<GET, POST>("/api/newperson", [](request &req, response &res){
        auto pStrategy = req.get_query_value("pStrategy");
        auto pType = req.get_query_value("pType");
        auto pName = req.get_query_value("pName");
        auto pStCamp = req.get_query_value("pStCamp");
        auto pStId = req.get_query_value("pStId");
        auto pEnCamp = req.get_query_value("pEnCamp");
        auto pEnId = req.get_query_value("pEnId");
        auto pStartTime = req.get_query_value("pStartTime");

        cout << pStrategy << " "
            << pType << " "
            << pName << " "
            << pStCamp << " "
            << pStId << " "
            << pEnCamp << " "
            << pEnId << " "
            << pStartTime << endl;

        // if (pName.empty()) {
        if (0) {
            res.set_status_and_content(status_type::bad_request, "Name Empty");
            return;
        }

        if (isReady == 1) {
            // delete_before();
        }
        isReady = 1;
        res.set_status_and_content(status_type::ok, "ready OK"); // tmp

        char *retStr;
        newPerson(retStr,
                stoi(string(pStrategy)),
                stoi(string(pType)),
                string(pName),
                stoi(string(pStCamp)),
                stoi(string(pStId)),
                stoi(string(pEnCamp)),
                stoi(string(pEnId)),
                stoi(string(pStartTime)));
        cout << "retStr: " << retStr << endl;
        res.set_status_and_content(status_type::ok, retStr, req_content_type::json);
        delete[] retStr;
    });

    server.set_http_handler<GET, POST>("/api/start", [](request &req, response &res) {
        if (isReady == 0) {
            res.set_status_and_content(status_type::bad_request, "ready defeat");
            return;
        }
        isRunning = 1;
        timeTot = 0.0;
        timeInit();

        char *retStr;
        updatePoint(retStr);
        cout << "retStr: " << retStr << endl;
        res.set_status_and_content(status_type::ok, retStr, req_content_type::json);
        delete[] retStr;
    });

    server.set_http_handler<GET, POST>("/api/pause", [](request &req, response &res){
        isRunning = 0;
        res.set_status_and_content(status_type::ok, "pause ok");
    });

    server.set_http_handler<GET, POST>("/api/update", [](request& req, response& res) {
        if (isRunning == 0) {
            res.set_status_and_content(status_type::bad_request, "ready defeat");
            return;
        }
        char* retStr;
        updatePoint(retStr);
        cout << "retStr: " << retStr << endl;
        res.set_status_and_content(status_type::ok, retStr, req_content_type::json);
        delete[] retStr;
    });

    server.set_http_handler<GET, POST>("/api/multiset", [](request &req, response &res){
        int multiNow;
        if (multiNow < MULTIDOWN || MULTIUP < multiNow) {
            res.set_status_and_content(status_type::bad_request, "bad request");
            return;
        }
        MULTISPEED = multiNow;
        cout << MULTISPEED << endl;

        char *retStr;
        updatePoint(retStr);
        cout << "retStr: " << retStr << endl;
        res.set_status_and_content(status_type::ok, retStr, req_content_type::json);
        delete[] retStr;
    });

    server.set_http_handler<GET, POST>("/api/multiinc", [](request &req, response &res){
        ++MULTISPEED;
        if (MULTISPEED > MULTIUP)
            MULTISPEED = MULTIUP;
        cout << MULTISPEED << endl;

        char *retStr;
        updatePoint(retStr);
        cout << "retStr: " << retStr << endl;
        res.set_status_and_content(status_type::ok, retStr, req_content_type::json);
        delete[] retStr;
    });

    server.set_http_handler<GET, POST>("/api/multidec", [](request &req, response &res){
        --MULTISPEED;
        if (MULTISPEED < MULTIDOWN)
            MULTISPEED = MULTIDOWN;
        cout << MULTISPEED << endl;
        
        char *retStr;
        updatePoint(retStr);
        cout << "retStr: " << retStr << endl;
        res.set_status_and_content(status_type::ok, retStr, req_content_type::json);
        delete[] retStr;
    });

    server.run();
}
