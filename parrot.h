#ifndef PARROT_H
#define PARROT_H

#include <iostream>
#include <vector>
#include <fstream>
#include <regex>
#include <filesystem>

class Parrot {
private:
    std::vector<std::streampos> matchA_positions;
    std::vector<std::streampos> matchB_positions;
    std::vector<std::string> matchB_targets;
    std::string filepath;
    std::ifstream file;

public:
    // Constructors
    Parrot(std::string file_path);
    Parrot(std::string file_path, std::string target, bool case_flag = false);

    // Destructor
    ~Parrot();

    // Methods
    void search(std::string target, bool case_flag = false);
    void close();
    size_t matchA() const;
    size_t matchB() const;
    size_t print(bool all_flag = false, bool verbose_flag = false);

    // Operator overload
    operator bool() const;
};

#endif // PARROT_H