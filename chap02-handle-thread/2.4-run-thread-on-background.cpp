#include <iostream>
#include <thread>
#include <vector>

using namespace std;

void open_document_and_display_gui(const string& s) {
    cout << "open...  " << s << endl;
}

void edit_document(std::string const& filename) {
    open_document_and_display_gui(filename);
    while (!done_editing()) {
        user_command cmd = get_user_input();
        if (cmd.type == open_new_document) {
            std::string const new_name = get_filename_from_user();
            std::thread t(edit_document, new_name);
            t.detach();
        } else {
            process_user_input(cmd);
        }
    }
}
void t1() {
    for (auto s : {"a", "b"}) edit_document(s);
}


int main(int argc, char const* argv[]) {
    t1();
    return 0;
}
