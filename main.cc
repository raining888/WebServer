#include "muduo/base/FileUtil.h"
#include "muduo/base/Logging.h"
#include "muduo/net/EventLoop.h"
#include "muduo/net/http/HttpRequest.h"
#include "muduo/net/http/HttpResponse.h"
#include "muduo/net/http/HttpServer.h"

#include <iostream>
#include <map>
#include <regex>

using namespace muduo;
using namespace muduo::net;

bool benchmark = false;

inline std::string file_extension(const std::string &path) {
  std::smatch m;
  auto pat = std::regex("\\.([a-zA-Z0-9]+)$");
  if (std::regex_search(path, m, pat)) {
    return m[1].str();
  }
  return std::string();
}

inline const char *find_content_type(const std::string &path) {
  auto ext = file_extension(path);
  if (ext == "txt") {
    return "text/plain";
  } else if (ext == "html") {
    return "text/html";
  } else if (ext == "css") {
    return "text/css";
  } else if (ext == "jpeg" || ext == "jpg") {
    return "image/jpg";
  } else if (ext == "png") {
    return "image/png";
  } else if (ext == "gif") {
    return "image/gif";
  } else if (ext == "svg") {
    return "image/svg+xml";
  } else if (ext == "ico") {
    return "image/x-icon";
  } else if (ext == "json") {
    return "application/json";
  } else if (ext == "pdf") {
    return "application/pdf";
  } else if (ext == "js") {
    return "application/javascript";
  } else if (ext == "xml") {
    return "application/xml";
  } else if (ext == "xhtml") {
    return "application/xhtml+xml";
  } else if (ext == "mp4") {
    return "video/mp4";
  }
  return nullptr;
}

void onRequest(const HttpRequest &req, HttpResponse *resp) {
  std::cout << "Headers " << req.methodString() << " " << req.path()
            << std::endl;
  if (!benchmark) {
    const std::map<string, string> &headers = req.headers();
    for (const auto &header : headers) {
      std::cout << header.first << ": " << header.second << std::endl;
    }
  }
  if (req.path() == "") {
    std::cout << "zero file" << std::endl;
    return;
  }
  resp->setStatusCode(HttpResponse::k200Ok);
  resp->setStatusMessage("OK");
  auto type = find_content_type(req.path());
  std::cout << type << std::endl;
  resp->setContentType(type);
  resp->addHeader("Server", "Blog");
  string res;
  int64_t size = 0;
  int err = FileUtil::readFile("./root" + req.path(), 102400000, &res, &size);
  if (err) {
    std::cout << "read file error" << std::endl;
  }
  resp->setBody(res);
}

int main(int argc, char *argv[]) {
  int numThreads = 0;
  if (argc > 1) {
    benchmark = true;
    Logger::setLogLevel(Logger::WARN);
    numThreads = atoi(argv[1]);
  }
  EventLoop loop;
  HttpServer server(&loop, InetAddress(8000), "dummy");
  server.setHttpCallback(onRequest);
  server.setThreadNum(numThreads);
  server.start();
  loop.loop();
  return 0;
}
