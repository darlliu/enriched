#include "pch.h"
#include <chrono>

int main(int argc, char **argv) {
  using namespace enriched;
  auto mydataset = std::make_unique<Dataset<symbol16, annotation16>>();
  mydataset->add_anno("anno1", "studying", "good!");
  mydataset->add_anno("anno2", "not studying", "bad!");
  mydataset->add_sym("m1", "male1", {"anno1"});
  for (int i = 0; i < 11; ++i)
    mydataset->add_sym("m" + std::to_string(2 + i),
                       "male " + std::to_string(2 + i), {"anno2"});
  for (int i = 1; i < 10; ++i)
    mydataset->add_sym("f" + std::to_string(i), "female " + std::to_string(i),
                       {"anno1"});
  for (int i = 10; i < 13; ++i)
    mydataset->add_sym("f" + std::to_string(i), "female " + std::to_string(i),
                       {"anno2"});
  mydataset->gen_mappings();
  auto enc1 = mydataset->encode_syms({"m1", "m4", "m6"});
  auto ans1 = mydataset->decode_syms(*enc1);
  auto enc2 = mydataset->encode_annos({"anno1", "anno2"});
  auto ans2 = mydataset->decode_annos(*enc2);
  SymSet<symbol16, annotation16> myset1({"m1", "m2", "m3", "m4", "m5", "m6",
                                         "m7", "m8", "m9", "m10", "m11", "m12"},
                                        *mydataset);
  auto res = myset1.get();
  auto res2 = myset1.get_mapped_mask();
  auto res3 = myset1.get_mask();
  auto f1 = fisher_t(1, 9, 11, 3);
  auto f2 = fisher_t(0, 10, 12, 2);
  SymSet<symbol16, annotation16> myset2({"f1", "f2", "f3", "f4", "f5", "f6",
                                         "f7", "f8", "f9", "f10", "f11", "f12"},
                                        *mydataset);
  auto rd = std::make_unique<ResultDataset>();
  fisher_test_ab(myset1, myset2, *mydataset, *rd);
  fisher_test(myset1, *mydataset, *rd);
  fold_change_test(myset1, *mydataset, *rd);
  rd->print();

  auto start = std::chrono::high_resolution_clock::now();
  auto rdsnp = std::make_unique<ResultDataset>();
  auto mysnp = std::make_unique<Dataset<symbol16, annotation16>>();
  load_annotations_plain(*mysnp, "D:/code/enriched/data/snp.anno.tsv");
  load_syms_with_mappings(*mysnp, "D:/code/enriched/data/snp.all.tsv");
  mysnp->gen_mappings();
  auto syms = load_syms_from_file("D:/code/enriched/data/snp.sig.tsv");
  SymSet<symbol16, annotation16> myset(syms, *mysnp);
  fisher_test(myset, *mysnp, *rdsnp);
  fold_change_test(myset, *mysnp, *rdsnp);

  auto mygo = std::make_unique<Dataset<symbol16, annotation16>>();
  load_annotations_plain(*mygo, "D:/code/enriched/data/go.anno.tsv");
  load_syms_with_mappings(*mygo, "D:/code/enriched/data/go.sym.tsv");
  mygo->gen_mappings();
  SymSet<symbol16, annotation16> circaset(
      {"ARNTL", "NR1D1", "SIRT1", "PPARG", "CLOCK"}, *mygo);
  fisher_test(circaset, *mygo, *rdsnp);
  fold_change_test(circaset, *mygo, *rdsnp);
  auto stop = std::chrono::high_resolution_clock::now();
  rdsnp->print();
  std::cout << "run time: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(stop -
                                                                     start)
                   .count()
            << " ms" << std::endl;
  auto fptr = fisher_test<SymSet<symbol15, Dataset<symbol15, annotation15>>, Dataset<symbol15, annotation15>>;
  return 0;
}