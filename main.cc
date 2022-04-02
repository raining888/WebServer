#include "muduo/base/FileUtil.h"
#include "muduo/base/Logging.h"
#include "muduo/net/EventLoop.h"
#include "muduo/net/http/HttpRequest.h"
#include "muduo/net/http/HttpResponse.h"
#include "muduo/net/http/HttpServer.h"

#include <iostream>
#include <map>
#include <regex>
#include <sys/stat.h>

using namespace muduo;
using namespace muduo::net;

bool benchmark = false;

inline bool isDir(const string& path)
{
  struct stat st;
  return stat(path.c_str(), &st) >= 0 && S_ISDIR(st.st_mode);
}

inline bool isFile(const string& path)
{
  struct stat st;
  return stat(path.c_str(), &st) >= 0 && S_ISREG(st.st_mode);
}

inline bool isValidPath(const string& path) {
  size_t level = 0;
  size_t i = 0;

  // Skip slash
  while (i < path.size() && path[i] == '/') {
    i++;
  }

  while (i < path.size()) {
    // Read component
    auto beg = i;
    while (i < path.size() && path[i] != '/') {
      i++;
    }

    auto len = i - beg;
    assert(len > 0);

    if (!path.compare(beg, len, ".")) {
      ;
    } else if (!path.compare(beg, len, "..")) {
      if (level == 0) {                return false;
      }
      level--;
    } else {
      level++;
    }

    // Skip slash
    while (i < path.size() && path[i] == '/') {
      i++;
    }
  }

  return true;
}

inline string fileExtension(const string &path) {
  std::smatch m;
  auto pat = std::regex("\\.([a-zA-Z0-9]+)$");
  if (std::regex_search(path, m, pat)) {
    return m[1].str();
  }
  return std::string();
}

inline const char *findContentType(const string &path) {
  auto ext = fileExtension(path);
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

  string rootDir = "./root";
  string path = rootDir + req.path();

  if (isDir(rootDir) &&
      isValidPath(req.path()) &&
      isFile(path)) {
    resp->setStatusCode(HttpResponse::k200Ok);
    resp->setStatusMessage("OK");
    auto type = findContentType(path);
    resp->setContentType(type);
    resp->addHeader("Server", "Blog");
    string res;
    struct stat st;
    stat(path.c_str(), &st);
    int err = FileUtil::readFile(path, 102400000, &res, &st.st_size);
    if (err) {
      std::cout << "read file error" << std::endl;
    }
    resp->setBody(res);
  } else {
    resp->setStatusCode(HttpResponse::k404NotFound);
    resp->setStatusMessage("Not Found");
    resp->setCloseConnection(true);
  }
}

int main(int argc, char *argv[]) {
  int numThreads = 0;
  if (argc > 1) {
    benchmark = true;
    Logger::setLogLevel(Logger::WARN);
    numThreads = atoi(argv[1]);
  }
  EventLoop loop;
  HttpServer server(&loop, InetAddress(12345), "webserver");
  server.setHttpCallback(onRequest);
  server.setThreadNum(numThreads);
  server.start();
  loop.loop();
  return 0;
}
