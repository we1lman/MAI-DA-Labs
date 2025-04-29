#include <iostream>
#include <string>
#include <cstdint>
#include <random>


std::string toLower(const std::string& s) {
    std::string r = s;
    for (char& c : r)
        if ('A' <= c && c <= 'Z')
            c = char(c - 'A' + 'a');
    return r;
}

class Treap {
private:
    struct Node {
        std::string key_;
        uint64_t value_;
        uint64_t priority_;

        Node* left_;
        Node* right_;

        Node(const std::string& k, uint64_t v, uint64_t p)
            : key_(k), value_(v), priority_(p), left_(nullptr), right_(nullptr) {}
    };

    Node* root_;
    std::mt19937_64 rng;
    
    Node* rotateRight(Node* y) {
        Node* x = y->left_;

        y->left_ = x->right_;
        x->right_ = y;

        return x;
    }
    
    Node* rotateLeft(Node* x) {
        Node* y = x->right_;

        x->right_ = y->left_;
        y->left_ = x;

        return y;
    }

    Node* insert(Node* t, const std::string& k, uint64_t v, bool& ok) {
        if (!t) {
            ok = true;
            return new Node(k, v, rng());
        }

        if (k == t->key_) {
            ok = false;
            return t;
        }

        if (k < t->key_) {
            t->left_ = insert(t->left_, k, v, ok);

            if (t->left_->priority_ > t->priority_)
                t = rotateRight(t);
        } else {
            t->right_ = insert(t->right_, k, v, ok);

            if (t->right_->priority_ > t->priority_)
                t = rotateLeft(t);
        }

        return t;
    }

    Node* remove(Node* t, const std::string& k, bool& ok) {
        if (!t) {
            ok = false;
            return nullptr;
        }

        if (k < t->key_) {
            t->left_ = remove(t->left_, k, ok);
        } 
        else if (k > t->key_) {
            t->right_ = remove(t->right_, k, ok);
        } 
        else {
            ok = true;
            
            if (!t->left_ && !t->right_) {
                delete t;
                return nullptr;
            }
            
            if (!t->right_ || (t->left_ && t->left_->priority_ > t->right_->priority_)) {
                t = rotateRight(t);
                t->right_ = remove(t->right_, k, ok);
            } else {
                t = rotateLeft(t);
                t->left_ = remove(t->left_, k, ok);
            }
        }

        return t;
    }

    Node* find(Node* t, const std::string& k) const {
        if (!t) return nullptr;

        if (k == t->key_) return t;
        
        return (k < t->key_ ? find(t->left_, k) : find(t->right_, k));
    }

    void destroy(Node* t) {
        if (!t) return;

        destroy(t->left_);
        destroy(t->right_);

        delete t;
    }

public:
    Treap() : root_(nullptr), rng(std::random_device{}()) {}
    ~Treap() { destroy(root_); }

    bool insert(const std::string& k, uint64_t v) {
        bool ok = false;
        root_ = insert(root_, k, v, ok);

        return ok;
    }

    bool remove(const std::string& k) {
        bool ok = false;
        root_ = remove(root_, k, ok);

        return ok;
    }

    Node* find(const std::string& k) const {
        return find(root_, k);
    }
};

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    Treap dict;
    std::string cmd, word;

    while (std::cin >> cmd) {
        if (cmd == "+") {
            uint64_t num;
            std::cin >> word >> num;
            word = toLower(word);

            std::cout << (dict.insert(word, num) ? "OK\n" : "Exist\n");
        } 
        else if (cmd == "-") {
            std::cin >> word;
            word = toLower(word);

            std::cout << (dict.remove(word) ? "OK\n" : "NoSuchWord\n");
        } 
        else {
            word = toLower(cmd);
            auto node = dict.find(word);

            if (node)
                std::cout << "OK: " << node->value_ << "\n";
            else
                std::cout << "NoSuchWord\n";
        }
    }
    
    return 0;
}
