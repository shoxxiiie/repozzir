#include <iostream>

template <typename T>
class UniquePtr {
private:
    T* ptr;

public:
    explicit UniquePtr(T* p = nullptr) : ptr(p) {}

    ~UniquePtr() { delete ptr; }

    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    UniquePtr(UniquePtr&& other) noexcept : ptr(other.ptr) {
        other.ptr = nullptr;
    }

    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (this != &other) {
            delete ptr;
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }

    T* get() const { return ptr; }
    T& operator*() const { return *ptr; }
    T* operator->() const { return ptr; }

    void reset(T* p = nullptr) {
        delete ptr;
        ptr = p;
    }

    T* release() {
        T* temp = ptr;
        ptr = nullptr;
        return temp;
    }
};

template <typename T>
class SharedPtr {
private:
    T* ptr;
    int* ref_count;

    void cleanup() {
        if (ref_count) {
            (*ref_count)--;
            if (*ref_count == 0) {
                delete ptr;
                delete ref_count;
            }
        }
    }

public:
    explicit SharedPtr(T* p = nullptr) : ptr(p), ref_count(p ? new int(1) : nullptr) {}

    ~SharedPtr() { cleanup(); }

    SharedPtr(const SharedPtr& other) : ptr(other.ptr), ref_count(other.ref_count) {
        if (ref_count) {
            (*ref_count)++;
        }
    }

    SharedPtr& operator=(const SharedPtr& other) {
        if (this != &other) {
            cleanup();
            ptr = other.ptr;
            ref_count = other.ref_count;
            if (ref_count) {
                (*ref_count)++;
            }
        }
        return *this;
    }

    T* get() const { return ptr; }
    T& operator*() const { return *ptr; }
    T* operator->() const { return ptr; }

    int use_count() const { return ref_count ? *ref_count : 0; }
};

struct Test {
    int value;
    Test(int v) : value(v) {}
};

int main() {
    {
        UniquePtr<Test> uptr1(new Test(10));
        std::cout << "UniquePtr: " << uptr1->value << "\n";
        UniquePtr<Test> uptr2 = std::move(uptr1);
        if (!uptr1.get()) std::cout << "UniquePtr moved successfully\n";
    }

    {
        SharedPtr<Test> sptr1(new Test(20));
        std::cout << "SharedPtr: " << sptr1->value << ", count: " << sptr1.use_count() << "\n";
        SharedPtr<Test> sptr2 = sptr1;
        std::cout << "SharedPtr: " << sptr2->value << ", count: " << sptr1.use_count() << "\n";
    }

    return 0;
}
