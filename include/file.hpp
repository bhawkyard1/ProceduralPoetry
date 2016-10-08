#ifndef FILE_HPP
#define FILE_HPP

#include <fstream>
#include <string>
#include <sstream>
#include <vector>

std::vector<std::string> getSources();

std::vector<std::string> loadMarkovSource(const std::string _path);

#endif
