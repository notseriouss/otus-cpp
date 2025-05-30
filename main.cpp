#include <iostream>
#include <map>
#include <memory>
#include <functional>
#include <stdexcept>
#include <cstddef>

int factorial(int n) {
    return (n <= 1) ? 1 : n * factorial(n - 1);
}

template <typename Map>
void print_map(const Map& map) {
    for (const auto& pair : map) {
        std::cout << pair.first << " " << pair.second << std::endl;
    }
}


template <typename T>
struct customAllocator {
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using propagate_on_container_move_assignment = std::true_type;
    using is_always_equal = std::false_type;

    customAllocator() = default;
    template <typename U>
    customAllocator(const customAllocator<U>&) {}

    T* allocate(size_type n) {
        std::cout << "Allocating " << n << " elements of size " << sizeof(T) << "\n";
        return static_cast<T*>(::operator new(n * sizeof(T)));
    }

    void deallocate(T* p, size_type n) {
        std::cout << "Deallocating " << n << " elements\n";
        ::operator delete(p);
    }

    template <typename U, typename... Args>
    void construct(U* p, Args&&... args) {
        new (p) U(std::forward<Args>(args)...);
    }

    template <typename U>
    void destroy(U* p) {
        p->~U();
    }
};

template <typename T1, typename T2>
bool operator==(const customAllocator<T1>&, const customAllocator<T2>&) { return true; }

template <typename T1, typename T2>
bool operator!=(const customAllocator<T1>&, const customAllocator<T2>&) { return false; }

template <typename Key, typename T>
struct my_map_node {
    std::pair<const Key, T> data;
    my_map_node* parent;
    my_map_node* left;
    my_map_node* right;
    bool color;  

    my_map_node(const Key& k, const T& v, my_map_node* p, my_map_node* l, my_map_node* r, bool c)
        : data(k, v), parent(p), left(l), right(r), color(c) {}
};

template <typename Key, typename T>
class MapIterator {
private:
    using Node = my_map_node<Key, T>;
    Node* current;
    Node* nil;

public:
    MapIterator(Node* node, Node* nil_node) : current(node), nil(nil_node) {}

    std::pair<const Key, T>& operator*() const { return current->data; }
    std::pair<const Key, T>* operator->() const { return &current->data; }

    MapIterator& operator++() {
        if (current == nil) return *this;
        
        if (current->right != nil) {
            current = current->right;
            while (current->left != nil)
                current = current->left;
        } else {
            Node* p = current->parent;
            while (p != nil && current == p->right) {
                current = p;
                p = p->parent;
            }
            current = p;
        }
        return *this;
    }

    bool operator==(const MapIterator& other) const { 
        return current == other.current; 
    }
    bool operator!=(const MapIterator& other) const { 
        return !(*this == other); 
    }
};

template <typename Key, typename T>
class my_map_const_Iterator {
private:
    using Node = my_map_node<Key, T>;
    const Node* current;
    const Node* nil;

public:
    my_map_const_Iterator(const Node* node, const Node* nil_node) : current(node), nil(nil_node) {}
          
    const std::pair<const Key, T>& operator*() const { return current->data; }
    const std::pair<const Key, T>* operator->() const { return &current->data; }

    my_map_const_Iterator& operator++() {
        if (current == nil) return *this;
        
        if (current->right != nil) {
            current = current->right;
            while (current->left != nil)
                current = current->left;
        } else {
            const Node* p = current->parent;
            while (p != nil && current == p->right) {
                current = p;
                p = p->parent;
            }
            current = p;
        }
        return *this;
    }

    bool operator==(const my_map_const_Iterator& other) const { 
        return current == other.current; 
    }
    bool operator!=(const my_map_const_Iterator& other) const { 
        return !(*this == other); 
    }
};


template <
    typename Key,
    typename T,
    typename Compare = std::less<Key>,
    typename Allocator = std::allocator<std::pair<const Key, T>>
>
class my_map {
public:
    using key_type = Key;
    using mapped_type = T;
    using value_type = std::pair<const Key, T>;
    using size_type = size_t;
    using iterator = MapIterator<Key, T>;
    using const_iterator = my_map_const_Iterator<Key, T>;

private:
    using Node = my_map_node<Key, T>;
    using NodeAllocator = typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;
    using AllocTraits = std::allocator_traits<NodeAllocator>;

    Node* root;
    Node* nil;  
    Compare comp;
    NodeAllocator alloc;

    Node* createNode(const Key& key, const T& value) {
        Node* node = AllocTraits::allocate(alloc, 1);
        try {
            AllocTraits::construct(alloc, node, key, value, nil, nil, nil, true);
        } catch (...) {
            AllocTraits::deallocate(alloc, node, 1);
            throw;
        }
        return node;
    }

    void destroyNode(Node* node) {
        AllocTraits::destroy(alloc, node);
        AllocTraits::deallocate(alloc, node, 1);
    }

    void leftRotate(Node* x) {
        Node* y = x->right;
        x->right = y->left;
        if (y->left != nil)
            y->left->parent = x;
        
        y->parent = x->parent;
        if (x->parent == nil)
            root = y;
        else if (x == x->parent->left)
            x->parent->left = y;
        else
            x->parent->right = y;
        
        y->left = x;
        x->parent = y;
    }

    void rightRotate(Node* x) {
        Node* y = x->left;
        x->left = y->right;
        if (y->right != nil)
            y->right->parent = x;
        
        y->parent = x->parent;
        if (x->parent == nil)
            root = y;
        else if (x == x->parent->right)
            x->parent->right = y;
        else
            x->parent->left = y;
        
        y->right = x;
        x->parent = y;
    }

    void fixInsert(Node* z) {
        while (z->parent->color) {  
            if (z->parent == z->parent->parent->left) {
                Node* y = z->parent->parent->right;
                if (y->color) {  
                    z->parent->color = false;
                    y->color = false;
                    z->parent->parent->color = true;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->right) {  
                        z = z->parent;
                        leftRotate(z);
                    }
                    z->parent->color = false;
                    z->parent->parent->color = true;
                    rightRotate(z->parent->parent);
                }
            } else {  
                Node* y = z->parent->parent->left;
                if (y->color) {
                    z->parent->color = false;
                    y->color = false;
                    z->parent->parent->color = true;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->left) {
                        z = z->parent;
                        rightRotate(z);
                    }
                    z->parent->color = false;
                    z->parent->parent->color = true;
                    leftRotate(z->parent->parent);
                }
            }
        }
        root->color = false;  
    }

    void clearTree(Node* node) {
        if (node != nil) {
            clearTree(node->left);
            clearTree(node->right);
            destroyNode(node);
        }
    }

    Node* findNode(const Key& key) const {
        Node* current = root;
        while (current != nil) {
            if (comp(key, current->data.first)) {
                current = current->left;
            } else if (comp(current->data.first, key)) {
                current = current->right;
            } else {
                return current;
            }
        }
        return nil;
    }

public:
    my_map(const Compare& comp = Compare(), const Allocator& a = Allocator())
        : comp(comp), alloc(a) {
        nil = AllocTraits::allocate(alloc, 1);
        AllocTraits::construct(alloc, nil, Key(), T(), nil, nil, nil, false);
        nil->parent = nil;
        root = nil;
    }

    my_map(const my_map&) = delete;

    ~my_map() {
        clearTree(root);
        AllocTraits::destroy(alloc, nil);
        AllocTraits::deallocate(alloc, nil, 1);
    }

    void insert(const Key& key, const T& value) {
        Node* z = createNode(key, value);
        Node* y = nil;
        Node* x = root;

        while (x != nil) {
            y = x;
            if (comp(key, x->data.first)) {
                x = x->left;
            } else if (comp(x->data.first, key)) {
                x = x->right;
            } else {  
                destroyNode(z);
                return;  
            }
        }

        z->parent = y;
        if (y == nil) {
            root = z;
        } else if (comp(key, y->data.first)) {
            y->left = z;
        } else {
            y->right = z;
        }

        fixInsert(z);
    }

    iterator begin() {
        Node* node = root;
        while (node != nil && node->left != nil)
            node = node->left;
        return iterator(node, nil);
    }

    const_iterator begin() const {
        Node* node = root;
        while (node != nil && node->left != nil)
            node = node->left;
        return const_iterator(node, nil);
    }

    iterator end() { 
        return iterator(nil, nil); 
    }

    const_iterator end() const { 
        return const_iterator(nil, nil); 
    }

    iterator find(const Key& key) {
        return iterator(findNode(key), nil);
    }

    const_iterator find(const Key& key) const {
        return const_iterator(findNode(key), nil);
    }
    
    T& at(const Key& key) {
        Node* node = findNode(key);
        if (node == nil) {
            throw std::out_of_range("Key not found");
        }
        return node->data.second;
    }

    T& operator[](const Key& key) {
        Node* node = findNode(key);
        if (node == nil) {
            insert(key, T());
            node = findNode(key);
        }
        return node->data.second;
    }
};

int main() {
    std::cout << "std::map with default allocator\n";
    std::map<int, int> first_one;
    for (int i = 0; i < 10; ++i) {
        first_one[i] = factorial(i);
    }
    print_map(first_one);

    std::cout << "std::map with custom allocator\n";
    std::map<int, int, std::less<int>, customAllocator<std::pair<const int, int>>> second_one;
    for (int i = 0; i < 10; ++i) {
        second_one[i] = factorial(i);
    }
    print_map(second_one);

    std::cout << "MyMap with default allocator\n";
    my_map<int, int, std::less<int>> third_one;
    for (int i = 0; i < 10; ++i) {
        third_one.insert(i, factorial(i));
    }
    print_map(third_one);

    std::cout << "MyMap with custom allocator\n";
    my_map<int, int, std::less<int>, customAllocator<std::pair<const int, int>>> fourth_one;
    for (int i = 0; i < 10; ++i) {
        fourth_one.insert(i, factorial(i));
    }
    print_map(fourth_one);

    return 0;
}
