#include "data.hpp"
#include "pch.h"
#include "stats.hpp"

int main(int argc, char **argv) {
  using namespace enriched;
  Dataset<symbol16, annotation16> mydataset;
  mydataset.add_anno("anno1", "studying", "good!");
  mydataset.add_anno("anno2", "not studying", "bad!");
  mydataset.add_sym("m1", "male1", { "anno1"});
  for (int i = 0; i < 11; ++i)
	  mydataset.add_sym("m" + std::to_string(2 + i), "male " + std::to_string(2 + i), {"anno2"});
  for (int i = 1; i < 10; ++i)
	  mydataset.add_sym("f" + std::to_string(i), "female " + std::to_string(i), { "anno1" });
  for (int i = 10; i < 13; ++i)
	  mydataset.add_sym("f" + std::to_string(i), "female " + std::to_string(i), { "anno2" });
  mydataset.gen_mappings();
  auto enc1 = mydataset.encode_syms({"m1", "m4", "m6"});
  auto ans1 = mydataset.decode_syms(enc1);
  auto enc2 = mydataset.encode_annos({"anno1", "anno2"});
  auto ans2 = mydataset.decode_annos(enc2);
  SymSet<symbol16, annotation16> myset1({"m1", "m2", "m3", "m4", "m5", "m6", "m7", "m8", "m9", "m10", "m11", "m12"}, mydataset);
  auto res = myset1.get();
  auto res2 = myset1.get_mapped_mask();
  auto res3 = myset1.get_mask();
  auto f1 = fisher_t(1, 9, 11, 3);
  auto f2 = fisher_t(0, 10, 12, 2);
  SymSet<symbol16, annotation16> myset2({ "f1", "f2", "f3", "f4", "f5", "f6", "f7", "f8", "f9", "f10", "f11", "f12" }, mydataset);
  auto test_res = ab_test<SymSet<symbol16, annotation16>, Dataset<symbol16, annotation16>, fisher_t>(myset1, myset2, mydataset);
  return 0;
}