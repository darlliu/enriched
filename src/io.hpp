#ifndef IO
#define IO
#include <cstdio>
#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

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
	auto ss = std::stringstream(line);
    std::getline(ss, ids, '\t');
	std::getline(ss, name, '\t');
    std::getline(ss, desc, '\t');
    dataset.add_anno(ids, name, desc);
  }
}

template <typename D>
void load_syms_with_mappings(D &dataset, std::string fname) {}

#endif