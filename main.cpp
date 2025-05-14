#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

std::vector<std::string> split(const std::string &str, char d) {
    std::vector<std::string> r;
    std::string::size_type start = 0;
    std::string::size_type stop = str.find_first_of(d);
    while (stop != std::string::npos) {
        r.push_back(str.substr(start, stop - start));
        start = stop + 1;
        stop = str.find_first_of(d, start);
    }
    r.push_back(str.substr(start));
    return r;
}

int main() {
    try {
        std::vector<std::vector<int>> ip_pool;

        for (std::string line; std::getline(std::cin, line);) {
            std::vector<std::string> fields = split(line, '\t');
            std::vector<int> ip;
            for (const auto &s : split(fields.at(0), '.')) {
                ip.push_back(std::stoi(s));
            }
            ip_pool.push_back(ip);
        }

        std::sort(ip_pool.begin(), ip_pool.end(),
                  [](const std::vector<int> &a, const std::vector<int> &b) {
                      for (size_t i = 0; i < 4; ++i) {
                          if (a[i] != b[i]) {
                              return a[i] > b[i];
                          }
                      }
                      return false;
                  });

        auto print_ip = [](const std::vector<int> &ip) {
            for (size_t i = 0; i < ip.size(); ++i) {
                if (i != 0) {
                    std::cout << ".";
                }
                std::cout << ip[i];
            }
            std::cout << "\n";
        };

        for (const auto &ip : ip_pool) {
            print_ip(ip);
        }

        for (const auto &ip : ip_pool) {
            if (ip[0] == 1) {
                print_ip(ip);
            }
        }

        for (const auto &ip : ip_pool) {
            if (ip[0] == 46 && ip[1] == 70) {
                print_ip(ip);
            }
        }

        for (const auto &ip : ip_pool) {
            bool has_46 = false;
            for (int part : ip) {
                if (part == 46) {
                    has_46 = true;
                    break;
                }
            }
            if (has_46) {
                print_ip(ip);
            }
        }
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}

