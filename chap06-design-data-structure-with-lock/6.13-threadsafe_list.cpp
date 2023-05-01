#include <mutex>
#include <memory>

// 线程安全的单向链表类模板
template <typename T>
class threadsafe_list {
    struct Node {
        std::mutex m;
        std::shared_ptr<T> data;
        std::unique_ptr<Node> next;
        Node() : next(nullptr) {}
        Node(T const& value) : data(std::make_shared<T>(value)) {}
    };
    Node head;

public:
    threadsafe_list() {}
    ~threadsafe_list() {
        remove_if([](Node const&) { return true; });
    }

    threadsafe_list(threadsafe_list const&) = delete;
    threadsafe_list& operator=(threadsafe_list const&) = delete;

    void push_front(T const& value) {
        std::unique_ptr<Node> new_node(new Node(value));
        std::lock_guard<std::mutex> lk(head.m);
        new_node->next = std::move(head.next);
        head.next = std::move(new_node);
    }

    template <typename Function>
    void for_each(Function f) {
        Node* cur = &head;
        std::unique_lock<std::mutex> lk(head.m);
        while (Node* const next = cur->next.get()) {
            std::unique_lock<std::mutex> next_lk(next->m);
            lk.unlock();
            f(*next->data);
            cur = next;
            lk = std::move(next_lk);
        }
    }

    template <typename Predicate>
    std::shared_ptr<T> find_first_if(Predicate p) {
        Node* cur = &head;
        std::unique_lock<std::mutex> lk(head.m);
        while (Node* const next = cur->next.get()) {
            std::unique_lock<std::mutex> next_lk(next->m);
            lk.unlock();
            if (p(*next->data)) return next->data;
            cur = next;
            lk = std::move(next_lk);
        }
        return std::shared_ptr<T>();
    }

    template <typename Predicate>
    void remove_if(Predicate p) {
        Node* cur = &head;
        std::unique_lock<std::mutex> lk(head.m);
        while (Node* const next = cur->next.get()) {
            std::unique_lock<std::mutex> next_lk(next->m);
            if (p(*next->data)) {
                std::unique_ptr<Node> old_next = std::move(cur->next);
                cur->next = std::move(next->next);
                next_lk.unlock();
            } else {
                lk.unlock();
                cur = next;
                lk = std::move(next_lk);
            }
        }
    }
};

int main(int argc, char* argv[]) {
    threadsafe_list<int> tl;
    for (auto i : {1, 2, 3, 4, 5}) tl.push_front(i);

    return 0;
}
