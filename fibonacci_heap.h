#include <bits/stdc++.h>
using namespace std;

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
    
    void insert(T key){
        FibNode<T>* nuevo = new FibNode<T>(key);
        
        if (min == nullptr) {
            min = nuevo;
        } else {
            min = merge(min, nuevo); 
        }
        
        n++;
    }
    FibNode<T>* findMin(){
        //revisa si el heap esta vacio
        if(!min){
            cout << "Heap vacio :'v";
            return min;
        }
        return min;
    }

    FibNode<T>* link(FibNode<T>* x, FibNode<T>* y) {
        // Aseguramos que x sea menor
        if (y->key < x->key) {
            swap(x, y);
        }
        
        // como x es menor y es hijo de x y deja de estar marcado
        y->parent = x;
        y->marked = false;
        
        // "x" no tiene hijos, "y" se mete como unico elemento
        if (x->child == nullptr) {
            x->child = y;
            y->left = y;
            y->right = y;
        } else {
            // x ya tiene hijos: insertar y en la lista circular de hijos
            FibNode<T>* ultimo = x->child->left;
            
            ultimo->right = y;
            y->left = ultimo;
            y->right = x->child;
            x->child->left = y;
        }
        //grado aumenta tiene +1 hijos
        x->degree++;
        
        return x; 
    }

    //Operacion no util en el fibonacci_heap en si, pero util para algoritmos como Prim
    void meld(FibonacciHeap<T>& other) {
        // Este heap está vacío
        if (min == nullptr) {
            min = other.min;
            n = other.n;
            other.min = nullptr;
            other.n = 0;
            return;
        }
        
        // El otro hep esta vacio
        if (other.min == nullptr) {
            return;
        }
        
        // Ninguno esta vacio
        // Unir las listas circulares
        min = merge(min,other.min);
        
        // Actualizar contador
        n += other.n;
        
        // Vaciar el otro heap
        other.min = nullptr;
        other.n = 0;
    }

    //Funcion auxiliar usada en extractMin para asegurar que cada grado tenga como maximo un arbol
    void consolidate(){
        //Teoricamente log2(n) deberia ser suficiente pero pongo 10 para un margen de error (casos muy inusuales)
        int maxDegree = static_cast<int>(log2(n)) + 10;
        vector<FibNode<T>*> A(maxDegree, nullptr);
        
        //Extrar todas las raices para ir modificandolas
        vector<FibNode<T>*> raices;
        FibNode<T>* actual = min;
        if (actual != nullptr) {
            do {
                raices.push_back(actual);
                actual = actual->right;
            } while (actual != min);
        }

        //Procesamos linkiando los de mismo grado
        for (FibNode<T>* w : raices) {
            FibNode<T>* x = w;
            int d = x->degree;
            
            // Combinar árboles del mismo grado
            while (A[d] != nullptr) {
                FibNode<T>* y = A[d];
                x = link(x, y); 
                A[d] = nullptr;
                d++;
            }
            A[d] = x;
        }


        //Reconstruir
        min = nullptr;
        for (int i = 0; i < maxDegree; i++) {
            if (A[i] != nullptr) {
                //Limpiamos atributos para reinsertar
                A[i]->parent = nullptr;
                A[i]->left = A[i];
                A[i]->right = A[i];
                
                if (min == nullptr) {
                    min = A[i];
                } else {
                    //Insertar A[i] en la lista de raíces
                    min = merge(min, A[i]); 
                }
            }
        }


    }
    FibNode<T>* merge(FibNode<T>* a, FibNode<T>* b)
    {
        if(a == nullptr) return b;
        if(b == nullptr) return a;
        
        // Asegurar que 'a' tenga la menor key
        if(a->key > b->key) {
            FibNode<T>* temp = a;
            a = b;
            b = temp;
        }
        
        // Unir las listas circulares
        FibNode<T>* aRight = a->right;
        FibNode<T>* bLeft  = b->left;
        
        a->right = b;
        b->left = a;
        aRight->left = bLeft;
        bLeft->right = aRight;
        
        return a;  // Retorna el nodo con menor key
    }

    //Funcion para que los hijos dejen de apuntar al padre
    void unparent_all(FibNode<T>* z){
        if(z == nullptr) return;
        FibNode<T>* y = z;
        do {
            y->parent = nullptr;
            y = y->right;
        }while(y != z);
    }

    void removeFromCircularList(FibNode<T>* x)
    {
        if (x->right == x)
            return;
        FibNode<T>* l = x->left;
        FibNode<T>* r = x->right;
        l->right = r;
        r->left = l;
        x->left = x->right = x;
    }
    FibNode<T>* extractMin() {
        FibNode<T>* z = min;
        if(!z) return nullptr;

        //hijos solos
        unparent_all(z->child);
        //juntamos hijo al min
        merge(z,z->child);

        //si solo tiene un elemento, min = null
        if (z == z->right){
            min = nullptr;
        }else{
            min = z->right;
        }
        removeFromCircularList(z);
        if(min){
            consolidate();
        }
        n--;
        return z;
    }
    void decreaseKey(FibNode<T>* x, T k){
        x->key = k;
        FibNode<T>* z = x->parent;
        if(z && x->key < z->key){
            cut(x,z);
            cascadingCut(z);
        }
        if(x->key < min->key){
            min = x;
        }
    }
    void cut(FibNode<T>* x, FibNode<T>* z){
        if(z->child == x){
            if(x->right == x){
                z->child = nullptr; 
            } else {
                z->child = x->right; 
            }
        }
        removeFromCircularList(x);
        z->degree--;
        
        x->parent = nullptr;
        x->marked = false;
        merge(min,x);
    }
    void cascadingCut(FibNode<T>* z){
        FibNode<T>* y = z->parent;
        if(y){
            if(!z->marked){
                z->marked = true;
            } 
            else{
                cut(z,y);
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
};