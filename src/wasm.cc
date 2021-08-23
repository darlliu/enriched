#include "data.hpp"
#include "export.hpp"
#include "stats.hpp"
#include <emscripten/bind.h>

using namespace emscripten;
using namespace std;
using namespace enriched;
EMSCRIPTEN_BINDINGS(c) {
  class_<StandardDataset>("StandardDataset")
      .constructor<>()
      .function("get_anno", &StandardDataset::get_anno);
  function("standard_fisher_test", standard_fisher);
  function("standard_fisher_test_ab", standard_fisher_ab);
}