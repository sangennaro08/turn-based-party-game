#include <iostream> 
#include <drogon/drogon.h>

using namespace drogon;

using Callback = std::function<void (const HttpResponsePtr &)>;

int main()
{

    

    app().registerHandler("/", [](const HttpRequestPtr& req, Callback &&callback)
    {
        auto resp = HttpResponse::newHttpResponse();
        resp->setBody("Hello World");
        callback(resp);
    });

    return 0;
}