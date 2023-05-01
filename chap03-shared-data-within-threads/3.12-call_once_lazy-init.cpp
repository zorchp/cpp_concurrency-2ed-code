
#include <thread>
#include <memory>
#include <mutex>
using namespace std;
class P {
public:
    void do_something() {}
};

std::shared_ptr<P> res_ptr;
std::once_flag res_flag;

void init_resource() { res_ptr.reset(new P); }

void foo() {
    std::call_once(res_flag, init_resource);
    res_ptr->do_something();
}

class Info {};
class Data {};
class Handle {
public:
    Data receive_data() { return Data(); }
    void send_data(Data) {}
    Handle open(Info) { return Handle(); }
};

Handle connection_manager;

class X {
    Info details;
    Handle connection;
    std::once_flag connection_init_flag;
    void open_connection() { connection = connection_manager.open(details); }

public:
    X(Info const& _details) : details(_details) {}
    void send_data(Data const& data) {
        std::call_once(connection_init_flag, &X::open_connection, this);
        connection.send_data(data);
    }
    Data receive_data() {
        std::call_once(connection_init_flag, &X::open_connection, this);
        return connection.receive_data();
    }
};


void t1() {
    //
}

int main(int argc, char* argv[]) {
    t1();
    return 0;
}
