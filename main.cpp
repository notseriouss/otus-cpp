#include <type_traits>
#include <iostream>
#include <list>
#include <vector>
#include <string>
#include <tuple>

// type traits for containers (std::vector && std::list && std::tuple)
template<typename T>
struct is_container : std::false_type {};

template<typename... Args>
struct is_container<std::vector<Args...>> : std::true_type {};

template<typename... Args>
struct is_container<std::list<Args...>> : std::true_type {};


template<typename T>
struct is_tuple : std::false_type {};

template<typename... Args>
struct is_tuple<std::tuple<Args...>> : std::true_type {};


// type traits for std::string
template<class T>
struct is_string : std::false_type {};

template<typename CharT, typename Traits, typename Alloc>
struct is_string<std::basic_string<CharT, Traits, Alloc>> : std::true_type {};




// containers ((std::vector && std::list) || std::tuple)
template<typename T>
std::enable_if_t<is_container<T>::value>
print_ip(const T& container) {
    unsigned short counter = 0;
    for (const auto& item : container) {
        if (counter != 0) {
            std::cout << '.';
        }
        std::cout << item;
        ++counter;
    }
    std::cout << std::endl;
}

template<typename T>
std::enable_if_t<is_tuple<T>::value>
print_ip(const T& tuple) {
    std::apply([](auto&&... args) {
        bool is_first = true; 
        auto print_elem = [&](auto&& arg) {
            if (!is_first) {
                std::cout << '.'; 
            }
            std::cout << arg;
            is_first = false; 
        };
        (print_elem(args), ...);  
    }, tuple);
    std::cout << "\n";
}

// ints
template<typename T>
std::enable_if_t<std::is_integral_v<T>>
print_ip(const T& integral) {
    using UnsignedType = typename std::make_unsigned<T>::type;
    UnsignedType unsigned_value = static_cast<UnsignedType>(integral);
    size_t num_bytes = sizeof(T);
    
    for (int i = num_bytes - 1; i >= 0; --i) {

        int byte = (unsigned_value >> (i * 8)) & 0xFF;
        std::cout << byte;
        if (i > 0) {
            std::cout << ".";
        }
    }
    std::cout << std::endl;
}

// std::string
template<typename T>
std::enable_if_t<is_string<T>::value>
print_ip(const T& str) {
    std::cout << str << std::endl;
}


int main() {
    print_ip( int8_t{-1} ); // 255 
    print_ip( int16_t{0} ); // 0.0 
    print_ip( int32_t{2130706433} ); // 127.0.0.1 
    print_ip( int64_t{8875824491850138409} );// 123.45.67.89.101.112.131.41 
    print_ip( std::string{"Hello, World!"} ); // Hello, World! 
    print_ip( std::vector<int>{100, 200, 300, 400} ); // 100.200.300.400 
    print_ip( std::list<short>{400, 300, 200, 100} ); // 400.300.200.100 
    print_ip( std::make_tuple(123, 456, 789, 0) ); // 123.456.789.0

    return 0;
}

