#include "pch.h"
#include "data.hpp"

int main(int argc, char **argv) {
	using namespace enriched;
  Dataset<symbol16, annotation24> mydataset;
  mydataset.add_anno("anno1", "my anno 1", "some anno");
  mydataset.add_anno("anno2", "my anno 2", "some anno");
  mydataset.add_anno("anno3", "my anno 3", "some anno");
  mydataset.add_anno("anno4", "my anno 4", "some anno");
  mydataset.add_sym("sym1", "my sym1", {"anno1", "anno4"});
  mydataset.add_sym("sym2", "my sym2", {"anno2", "anno3"});
  mydataset.add_sym("sym3", "my sym3", {"anno3", "anno4"});
  mydataset.add_sym("sym4", "my sym4", {"anno1"});
  mydataset.gen_mappings();
  auto enc1 = mydataset.encode_syms({"sym1", "sym3"});
  auto ans1 = mydataset.decode_syms(enc1);
  auto enc2 = mydataset.encode_annos({"anno2", "anno4"});
  auto ans2 = mydataset.decode_annos(enc2);
  SymSet<symbol16, annotation24> myset1({"sym1", "sym4"}, mydataset);
  auto res = myset1.get();
  auto res2 = myset1.get_mapped_mask();
  return 0;
}