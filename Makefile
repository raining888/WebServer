BUILD:=./build
SRC:=.

CC:=g++

CXX_FLAGS:= -DCHECK_PTHREAD_RETURN_VALUE
CXX_FLAGS+= -D_FILE_OFFSET_BITS=64
CXX_FLAGS+= -Wall
CXX_FLAGS+= -Wextra
CXX_FLAGS+= -Werror
CXX_FLAGS+= -Wconversion
CXX_FLAGS+= -Wno-unused-parameter
CXX_FLAGS+= -Wold-style-cast
CXX_FLAGS+= -Woverloaded-virtual
CXX_FLAGS+= -Wpointer-arith
CXX_FLAGS+= -Wshadow
CXX_FLAGS+= -Wwrite-strings
CXX_FLAGS+= -march=native
CXX_FLAGS+= -std=c++11
CXX_FLAGS+= -rdynamic
CXX_FLAGS+= -O2
CXX_FLAGS+= -DNDEBUG

LDFLAGS:= -pthread -lrt

DEBUG:= -g

INCLUDE:=-I$(SRC)
INCLUDE+=-I/usr/include/boost

$(BUILD)/%.o: $(SRC)/%.cc
	$(shell mkdir -p $(dir $@))
	$(CC) $(CXX_FLAGS) $(DEBUG) $(INCLUDE) -c $< -o $@

$(BUILD)/muduo/base/%.o: $(SRC)/muduo/base/%.cc
	$(shell mkdir -p $(dir $@))
	$(CC) $(CXX_FLAGS) $(DEBUG) $(INCLUDE) -c $< -o $@

$(BUILD)/muduo/net/%.o: $(SRC)/muduo/net/%.cc
	$(shell mkdir -p $(dir $@))
	$(CC) $(CXX_FLAGS) $(DEBUG) $(INCLUDE) -c $< -o $@

$(BUILD)/muduo/net/poller/%.o: $(SRC)/muduo/net/poller/%.cc
	$(shell mkdir -p $(dir $@))
	$(CC) $(CXX_FLAGS) $(DEBUG) $(INCLUDE) -c $< -o $@

$(BUILD)/muduo/net/http/%.o: $(SRC)/muduo/net/http/%.cc
	$(shell mkdir -p $(dir $@))
	$(CC) $(CXX_FLAGS) $(DEBUG) $(INCLUDE) -c $< -o $@

$(BUILD)/webserver: \
	$(BUILD)/main.o \
	$(BUILD)/muduo/base/AsyncLogging.o \
	$(BUILD)/muduo/base/Condition.o \
	$(BUILD)/muduo/base/CountDownLatch.o \
	$(BUILD)/muduo/base/CurrentThread.o \
	$(BUILD)/muduo/base/Date.o \
	$(BUILD)/muduo/base/Exception.o \
	$(BUILD)/muduo/base/FileUtil.o \
	$(BUILD)/muduo/base/LogFile.o \
	$(BUILD)/muduo/base/Logging.o \
	$(BUILD)/muduo/base/LogStream.o \
	$(BUILD)/muduo/base/ProcessInfo.o \
	$(BUILD)/muduo/base/Thread.o \
	$(BUILD)/muduo/base/ThreadPool.o \
	$(BUILD)/muduo/base/Timestamp.o \
	$(BUILD)/muduo/base/TimeZone.o \
	$(BUILD)/muduo/net/http/HttpContext.o \
	$(BUILD)/muduo/net/http/HttpResponse.o \
	$(BUILD)/muduo/net/http/HttpServer.o \
	$(BUILD)/muduo/net/poller/DefaultPoller.o \
	$(BUILD)/muduo/net/poller/EPollPoller.o \
	$(BUILD)/muduo/net/poller/PollPoller.o \
	$(BUILD)/muduo/net/Acceptor.o \
	$(BUILD)/muduo/net/boilerplate.o \
	$(BUILD)/muduo/net/Buffer.o \
	$(BUILD)/muduo/net/Channel.o \
	$(BUILD)/muduo/net/Connector.o \
	$(BUILD)/muduo/net/EventLoop.o \
	$(BUILD)/muduo/net/EventLoopThread.o \
	$(BUILD)/muduo/net/EventLoopThreadPool.o \
	$(BUILD)/muduo/net/InetAddress.o \
	$(BUILD)/muduo/net/Poller.o \
	$(BUILD)/muduo/net/Socket.o \
	$(BUILD)/muduo/net/SocketsOps.o \
	$(BUILD)/muduo/net/TcpClient.o \
	$(BUILD)/muduo/net/TcpConnection.o \
	$(BUILD)/muduo/net/TcpServer.o \
	$(BUILD)/muduo/net/Timer.o \
	$(BUILD)/muduo/net/TimerQueue.o \
	
	$(shell mkdir -p $(dir $@))
	$(CC) $(LDFLAGS) $^ -o $@

.PHONY: clean
clean:
	-rm -rf $(BUILD)
