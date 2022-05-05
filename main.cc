#include <drogon/drogon.h>
#include "custom/utils.h"

using namespace drogon;

int main() {


    // some custom function
    {
        // update datetime
        drogon::Custom::update_time("172.168.100.79:8112", "/api/services/assistant/UserConfig/GetNow");
    }

    trantor::Logger::setLogLevel(trantor::Logger::kTrace);
    //drogon web
    {
        //Load config file
        drogon::app().loadConfigFile("../config.json");
        //Run HTTP framework,the method will block in the internal event loop
    }
    drogon::app().run();
    return 0;
}
