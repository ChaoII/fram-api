#include <drogon/drogon.h>

using namespace drogon;

int main() {

//    trantor::Logger::setLogLevel(trantor::Logger::kTrace);
    //drogon web
    //Load config file
    drogon::app().loadConfigFile("../config.json");
    //Run HTTP framework,the method will block in the internal event loop
    drogon::app().run();
    return 0;
}
