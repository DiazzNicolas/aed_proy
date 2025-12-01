#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <vector>
#include <queue>
#include <iostream>
#include <sstream>
#include <cmath>
using namespace std;
using namespace emscripten;

template<typename T>
struct FibNode {
    T key;
    int degree;
    bool marked;
    
    FibNode* parent;
    FibNode* child;
    FibNode* left;
    FibNode* right;
    
    FibNode(T k) : key(k), degree(0), marked(false),
                   parent(nullptr), child(nullptr) {
        left = right = this;
    }
};

template<typename T>
class FibonacciHeap {
private:
    FibNode<T>* min;
    int n;
    
public:
    FibonacciHeap() : min(nullptr), n(0) {}
    
    void insert(T key) {
        FibNode<T>* nuevo = new FibNode<T>(key);
        
        if (min == nullptr) {
            min = nuevo;
        } else {
            min = merge(min, nuevo);
        }
        
        n++;
    }
    
    FibNode<T>* findMin() {
        if (!min) {
            return nullptr;
        }
        return min;
    }

    FibNode<T>* link(FibNode<T>* x, FibNode<T>* y) {
        if (y->key < x->key) {
            swap(x, y);
        }
        
        y->parent = x;
        y->marked = false;
        
        if (x->child == nullptr) {
            x->child = y;
            y->left = y;
            y->right = y;
        } else {
            FibNode<T>* ultimo = x->child->left;
            
            ultimo->right = y;
            y->left = ultimo;
            y->right = x->child;
            x->child->left = y;
        }
        x->degree++;
        
        return x;
    }

    void consolidate() {
        int maxDegree = static_cast<int>(log2(n)) + 10;
        vector<FibNode<T>*> A(maxDegree, nullptr);
        
        vector<FibNode<T>*> raices;
        FibNode<T>* actual = min;
        if (actual != nullptr) {
            do {
                raices.push_back(actual);
                actual = actual->right;
            } while (actual != min);
        }

        for (FibNode<T>* w : raices) {
            FibNode<T>* x = w;
            int d = x->degree;
            
            while (A[d] != nullptr) {
                FibNode<T>* y = A[d];
                x = link(x, y);
                A[d] = nullptr;
                d++;
            }
            A[d] = x;
        }

        min = nullptr;
        for (int i = 0; i < maxDegree; i++) {
            if (A[i] != nullptr) {
                A[i]->parent = nullptr;
                A[i]->left = A[i];
                A[i]->right = A[i];
                
                if (min == nullptr) {
                    min = A[i];
                } else {
                    min = merge(min, A[i]);
                }
            }
        }
    }
    
    FibNode<T>* merge(FibNode<T>* a, FibNode<T>* b) {
        if (a == nullptr) return b;
        if (b == nullptr) return a;
        
        if (a->key > b->key) {
            FibNode<T>* temp = a;
            a = b;
            b = temp;
        }
        
        FibNode<T>* aRight = a->right;
        FibNode<T>* bLeft = b->left;
        
        a->right = b;
        b->left = a;
        aRight->left = bLeft;
        bLeft->right = aRight;
        
        return a;
    }

    void unparent_all(FibNode<T>* z) {
        if (z == nullptr) return;
        FibNode<T>* y = z;
        do {
            y->parent = nullptr;
            y = y->right;
        } while (y != z);
    }

    void removeFromCircularList(FibNode<T>* x) {
        if (x->right == x) return;
        FibNode<T>* l = x->left;
        FibNode<T>* r = x->right;
        l->right = r;
        r->left = l;
        x->left = x->right = x;
    }
    
    FibNode<T>* extractMin() {
        FibNode<T>* z = min;
        if (!z) return nullptr;

        unparent_all(z->child);
        merge(z, z->child);

        if (z == z->right) {
            min = nullptr;
        } else {
            min = z->right;
        }
        removeFromCircularList(z);
        if (min) {
            consolidate();
        }
        n--;
        return z;
    }
    
    void cut(FibNode<T>* x, FibNode<T>* z) {
        if (z->child == x) {
            if (x->right == x) {
                z->child = nullptr;
            } else {
                z->child = x->right;
            }
        }
        removeFromCircularList(x);
        z->degree--;
        
        x->parent = nullptr;
        x->marked = false;
        merge(min, x);
    }
    
    void cascadingCut(FibNode<T>* z) {
        FibNode<T>* y = z->parent;
        if (y) {
            if (!z->marked) {
                z->marked = true;
            } else {
                cut(z, y);
                cascadingCut(y);
            }
        }
    }
    
    FibNode<T>* findNode(T value) {
        if (min == nullptr) return nullptr;
        
        queue<FibNode<T>*> q;
        
        FibNode<T>* curr = min;
        do {
            q.push(curr);
            curr = curr->right;
        } while (curr != min);
        
        while (!q.empty()) {
            FibNode<T>* node = q.front();
            q.pop();
            
            if (node->key == value) return node;
            
            if (node->child) {
                FibNode<T>* child = node->child;
                do {
                    q.push(child);
                    child = child->right;
                } while (child != node->child);
            }
        }
        
        return nullptr;
    }
    
    void deleteByValue(T i) {
        FibNode<T>* x = findNode(i);
        if (x == nullptr) return;
        
        deleteNode(x);
    }
    
    bool decreaseKeyNode(FibNode<T>* x, T k) {
        if (x == nullptr || k >= x->key) return false;
        
        x->key = k;
        FibNode<T>* z = x->parent;
        
        if (z && x->key < z->key) {
            cut(x, z);
            cascadingCut(z);
        }
        
        if (x->key < min->key) {
            min = x;
        }
        
        return true;
    }
    
    bool decreaseKeyByValue(T oldValue, T newValue) {
        FibNode<T>* node = findNode(oldValue);
        if (node == nullptr) return false;
        return decreaseKeyNode(node, newValue);
    }
    
    void deleteNode(FibNode<T>* x) {
        if (x == nullptr) return;
        
        if (x == min) {
            extractMin();
            return;
        }
        
        FibNode<T>* p = x->parent;
        
        if (p != nullptr) {
            if (p->child == x) {
                if (x->right == x) {
                    p->child = nullptr;
                } else {
                    p->child = x->right;
                }
            }
            removeFromCircularList(x);
            p->degree--;
        } else {
            removeFromCircularList(x);
        }
        unparent_all(x->child);
        merge(min, x->child);
    
        delete x;
        n--;
    }

    ~FibonacciHeap() {
        if (min == nullptr) return;
        
        queue<FibNode<T>*> q;
        
        FibNode<T>* curr = min;
        do {
            q.push(curr);
            curr = curr->right;
        } while (curr != min);
        
        while (!q.empty()) {
            FibNode<T>* node = q.front();
            q.pop();
            
            if (node->child) {
                FibNode<T>* child = node->child;
                do {
                    q.push(child);
                    child = child->right;
                } while (child != node->child);
            }
            
            delete node;
        }
    }

    bool isEmpty() const {
        return min == nullptr;
    }
    
    int size() const {
        return n;
    }
    
    void clear() {
        if (min == nullptr) return;
        
        queue<FibNode<T>*> q;
        
        FibNode<T>* curr = min;
        do {
            q.push(curr);
            curr = curr->right;
        } while (curr != min);
        
        while (!q.empty()) {
            FibNode<T>* node = q.front();
            q.pop();
            
            if (node->child) {
                FibNode<T>* child = node->child;
                do {
                    q.push(child);
                    child = child->right;
                } while (child != node->child);
            }
            
            delete node;
        }
        
        min = nullptr;
        n = 0;
    }
    
    string serializeToJSON() {
        if (min == nullptr) return "[]";
        
        stringstream ss;
        ss << "[";
        
        vector<FibNode<T>*> roots;
        FibNode<T>* curr = min;
        do {
            roots.push_back(curr);
            curr = curr->right;
        } while (curr != min);
        
        for (size_t i = 0; i < roots.size(); i++) {
            if (i > 0) ss << ",";
            serializeNode(ss, roots[i]);
        }
        
        ss << "]";
        return ss.str();
    }
    
private:
    void serializeNode(stringstream& ss, FibNode<T>* node) {
        if (!node) return;
        
        ss << "{";
        ss << "\"key\":" << node->key << ",";
        ss << "\"degree\":" << node->degree << ",";
        ss << "\"marked\":" << (node->marked ? "true" : "false") << ",";
        ss << "\"isMin\":" << (node == min ? "true" : "false") << ",";
        ss << "\"children\":[";
        
        if (node->child) {
            vector<FibNode<T>*> children;
            FibNode<T>* child = node->child;
            do {
                children.push_back(child);
                child = child->right;
            } while (child != node->child);
            
            for (size_t i = 0; i < children.size(); i++) {
                if (i > 0) ss << ",";
                serializeNode(ss, children[i]);
            }
        }
        
        ss << "]}";
    }
};

class FibHeapWrapper {
private:
    FibonacciHeap<int> heap;
    
public:
    void insert(int key) {
        heap.insert(key);
    }
    
    int extractMin() {
        auto node = heap.extractMin();
        return node ? node->key : -1;
    }
    
    int findMin() {
        auto node = heap.findMin();
        return node ? node->key : -1;
    }
    
    void deleteByValue(int value) {
        heap.deleteByValue(value);
    }
    
    bool decreaseKey(int oldValue, int newValue) {
        return heap.decreaseKeyByValue(oldValue, newValue);
    }
    
    int size() {
        return heap.size();
    }
    
    bool isEmpty() {
        return heap.isEmpty();
    }
    
    void clear() {
        heap.clear();
    }
    
    string getStructure() {
        return heap.serializeToJSON();
    }
};

EMSCRIPTEN_BINDINGS(fibonacci_heap) {
    class_<FibHeapWrapper>("FibHeap")
        .constructor<>()
        .function("insert", &FibHeapWrapper::insert)
        .function("extractMin", &FibHeapWrapper::extractMin)
        .function("findMin", &FibHeapWrapper::findMin)
        .function("deleteByValue", &FibHeapWrapper::deleteByValue)
        .function("decreaseKey", &FibHeapWrapper::decreaseKey)
        .function("size", &FibHeapWrapper::size)
        .function("isEmpty", &FibHeapWrapper::isEmpty)
        .function("clear", &FibHeapWrapper::clear)
        .function("getStructure", &FibHeapWrapper::getStructure);
}