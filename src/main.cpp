#include "pch.h"
#include <chrono>

void print_result(std::tuple<std::string, std::vector<test_result>> res) {
  std::cout << "test result : " << std::get<0>(res) << std::endl;
  auto vec = std::get<1>(res);
  for (auto &r : vec) {
    std::cout << "name: " << r.name << "\t stat: " << r.stat
              << "\t enriched?: " << r.enriched << std::endl;
  }
  std::cout << "====================" << std::endl;
}

int main(int argc, char **argv) {
  auto start = std::chrono::high_resolution_clock::now();
  using namespace enriched;
  Dataset<symbol16, annotation16> mydataset;
  mydataset.add_anno("anno1", "studying", "good!");
  mydataset.add_anno("anno2", "not studying", "bad!");
  mydataset.add_sym("m1", "male1", {"anno1"});
  for (int i = 0; i < 11; ++i)
    mydataset.add_sym("m" + std::to_string(2 + i),
                      "male " + std::to_string(2 + i), {"anno2"});
  for (int i = 1; i < 10; ++i)
    mydataset.add_sym("f" + std::to_string(i), "female " + std::to_string(i),
                      {"anno1"});
  for (int i = 10; i < 13; ++i)
    mydataset.add_sym("f" + std::to_string(i), "female " + std::to_string(i),
                      {"anno2"});
  mydataset.gen_mappings();
  auto enc1 = mydataset.encode_syms({"m1", "m4", "m6"});
  auto ans1 = mydataset.decode_syms(*enc1);
  auto enc2 = mydataset.encode_annos({"anno1", "anno2"});
  auto ans2 = mydataset.decode_annos(*enc2);
  SymSet<symbol16, annotation16> myset1({"m1", "m2", "m3", "m4", "m5", "m6",
                                         "m7", "m8", "m9", "m10", "m11", "m12"},
                                        mydataset);
  auto res = myset1.get();
  auto res2 = myset1.get_mapped_mask();
  auto res3 = myset1.get_mask();
  auto f1 = fisher_t(1, 9, 11, 3);
  auto f2 = fisher_t(0, 10, 12, 2);
  SymSet<symbol16, annotation16> myset2({"f1", "f2", "f3", "f4", "f5", "f6",
                                         "f7", "f8", "f9", "f10", "f11", "f12"},
                                        mydataset);
  auto test_res = fisher_test(myset1, myset2, mydataset);
  print_result(test_res);

  auto test_res2 = fisher_test(myset1, mydataset);
  print_result(test_res2);

  auto test_res3 = fold_change_test(myset1, mydataset);
  print_result(test_res3);

  Dataset<symbol18, annotation18> mysnp;
  load_annotations_plain(mysnp, "D:/code/enriched/data/snp.anno.tsv");
  load_syms_with_mappings(mysnp, "D:/code/enriched/data/snp.all.tsv");
  mysnp.gen_mappings();
  auto syms = load_syms_from_file("D:/code/enriched/data/snp.sig.tsv");
  SymSet<symbol18, annotation18> myset(syms, mysnp);
  auto test_ress1 = fisher_test(myset, mysnp);
  print_result(test_ress1);
  auto test_ress2 = fold_change_test(myset, mysnp);
  print_result(test_ress2);

  Dataset<symbol18, annotation18> mygo;
  load_annotations_plain(mygo, "D:/code/enriched/data/go.anno.tsv");
  load_syms_with_mappings(mygo, "D:/code/enriched/data/go.sym.tsv");
  mygo.gen_mappings();
  SymSet<symbol18, annotation18> circaset(
      {"ARNTL", "NR1D1", "SIRT1", "PPARG", "CLOCK"}, mygo);
  auto test_res4 = fisher_test(circaset, mygo);
  print_result(test_res4);
  auto test_res5 = fold_change_test(circaset, mygo);
  print_result(test_res5);
  auto stop = std::chrono::high_resolution_clock::now();
  std::cout << "run time: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(stop -
                                                                     start)
                   .count()
            << " ms" << std::endl;
  return 0;
}