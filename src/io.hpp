#ifndef IO
#define IO
#include <cstdio>
#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

template <typename D>
void load_annotations_plain(D &dataset, std::string fname) {
  std::ifstream ifs(fname);
  if (!ifs.is_open()) {
    throw(std::runtime_error("Wrong filename provided:" + fname));
  }
  std::string line, ids, name, desc;
  while (std::getline(ifs, line)) {
    if (line.size() == 0) {
      continue;
    }
    std::stringstream ss(line);
    std::getline(ss, ids, '\t');
    std::getline(ss, name, '\t');
    std::getline(ss, desc, '\t');
    dataset.add_anno(ids, name, desc);
  }
}

template <typename D>
void load_syms_with_mappings(D &dataset, std::string fname) {
  std::ifstream ifs(fname);
  if (!ifs.is_open()) {
    throw(std::runtime_error("Wrong filename provided:" + fname));
  }
  std::string line, sym, tmp, to;
  std::vector<std::string> mappings;
  while (std::getline(ifs, line)) {
    if (line.size() == 0) {
      continue;
    }
    std::stringstream ss(line);
    std::getline(ss, sym, '\t');
    std::getline(ss, tmp, '\t');
    mappings.clear();
    std::stringstream ss2(tmp);
    while (std::getline(ss2, to, ',')) {
      mappings.push_back(std::move(to));
    }
    dataset.add_sym(sym, sym, mappings);
  }
}

inline std::vector<std::string> load_syms_from_file (std::string fname) {
  std::vector<std::string> out;
  std::ifstream ifs(fname);
  if (!ifs.is_open()) {
    throw(std::runtime_error("Wrong filename provided:" + fname));
  }
  std::string line;
  while (std::getline(ifs, line)) {
    if (line.size() == 0) {
      continue;
    }
    out.push_back(std::move(line));
  }
  return out;
}
#endif