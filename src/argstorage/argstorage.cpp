#include <format>
#include <iostream>
#include <tuple>

class MyClass {
private:
  int32_t val_;

public:
  MyClass(int32_t val) : val_(val) {
    std::cout << std::format("{} {}", __func__, val_) << std::endl;
  }
  MyClass(const MyClass& my_class) : val_(my_class.val_) {
    std::cout << std::format("copy {} {}", __func__, val_) << std::endl;
  }
  MyClass(MyClass&& my_class) : val_(my_class.val_) {
    std::cout << std::format("move {} {}", __func__, val_) << std::endl;
    my_class.val_ = -1;
  }
  ~MyClass() {
    std::cout << std::format("{} {}", __func__, val_) << std::endl;
  }
  friend std::ostream& operator<<(std::ostream& os, const MyClass& myclass);
};

std::ostream& operator<<(std::ostream& os, const MyClass& myclass) {
  os << std::format("MyClass: {}", myclass.val_);
  return os;
}

template<typename T>
using selector = std::conditional_t<std::is_lvalue_reference_v<T>, T, std::remove_reference_t<T>>;

template<typename... Args>
class ArgStorage {
private:
  std::tuple<selector<Args>...> storage_;

public:
  ArgStorage(Args&&... args) : storage_(std::forward<Args>(args)...) {
    std::cout << std::format("{}", __func__) << std::endl;
  }
  void Print() {
    std::cout << "[";
    std::apply([](auto&&... args) { ((std::cout << args << ", "), ...); }, storage_);
    std::cout << "]" << std::endl;
  }
};

template<typename... Args>
class RefStorage {
private:
  std::tuple<const Args&...> storage_;

public:
  RefStorage(const Args&... args) : storage_(args...) {
    std::cout << __func__ << std::endl;
  }
  void Print() {
    std::cout << "[";
    std::apply([](auto&&... args) { ((std::cout << args << ", "), ...); }, storage_);
    std::cout << "]" << std::endl;
  }
};

template<typename... Args>
ArgStorage(Args&&...) -> ArgStorage<Args...>;

auto main() -> int32_t {
  std::cout << "--- ArgStorage ---" << std::endl;
  {
    int32_t a = 10;
    MyClass b(1);

    std::cout << "----" << std::endl;
    ArgStorage arg_storage(a, b, 10, MyClass(2));
    arg_storage.Print();
    std::cout << "----" << std::endl;
  }

  std::cout << "--- RefStorage ---" << std::endl;
  {
    int32_t a = 10;
    MyClass b(3);

    std::cout << "----" << std::endl;
    RefStorage ref_storage(a, b, 10, MyClass(4));
    ref_storage.Print();
    std::cout << "----" << std::endl;
  }
  std::cout << "end" << std::endl;
}