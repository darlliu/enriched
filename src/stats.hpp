#ifndef STATS
#define STATS
#include <string>
#include <tuple>
#include <vector>
constexpr double fisher_t(const unsigned &a, const unsigned &b,
                          const unsigned &c, const unsigned &d) {
  double ans = 1.0;
  unsigned f1 = 1, f2 = 1, f3 = 1, f4 = 1;
  /*
   *  fisher_t = (a+b)!/a! *(a+c)!/c! * (c+d)!/d! (b+d)!/b! / n!
   *  let i from 1 -> n, when i > a, i > b, i > c etc
   */
  for (unsigned i = 1; i <= a + b + c + d; ++i) {
    ans /= i;
    f1 = (i > a) && (i <= a + b) ? i : 1;
    ans *= f1;
    f2 = (i > b) && (i <= b + d) ? i : 1;
    ans *= f2;
    f3 = (i > c) && (i <= a + c) ? i : 1;
    ans *= f3;
    f4 = (i > d) && (i <= c + d) ? i : 1;
    ans *= f4;
  }
  return ans;
}

constexpr double fold_change(const unsigned &a, const unsigned &b,
                             const unsigned &c, const unsigned &d) {
  double ans = 1.0;
  if (a + c == 0 || b + d == 0)
    return ans;
  double r1 = a / static_cast<double>(a + c);
  double r2 = b / static_cast<double>(b + d);
  if (r2 == 0)
    return ans;
  return r1 / r2;
}

struct test_result {
  std::string name;
  double stat;
  bool enriched;
};

template <typename S, typename D, decltype(fisher_t) fn>
std::tuple<std::string, std::vector<test_result>>
ab_test(const S &test_set, const S &control_set, const D &dataset,
        std::string test_name = "fisher") {
  std::vector<test_result> out;
  // 1, get all hot annotations from test set
  auto hot_mask = test_set.get_mapped_mask();
  // 2, for each annotation
  for (unsigned i = 0; i < hot_mask.size(); ++i) {
    if (!hot_mask.test(i))
      continue;
    // 3, build contingency table
    const auto anno = dataset.get_anno(i);
    auto total_mask = anno->get_mask();
    decltype(total_mask) test_mask = test_set.get_mask(),
                         control_mask = control_set.get_mask();
    unsigned total_test = test_mask.count(),
             total_control = control_mask.count();
    test_mask &= total_mask;
    control_mask &= total_mask;
    // no 4, add test statistics by calling fn
    double stat = fn(test_mask.count(), control_mask.count(),
                     total_test - test_mask.count(),
                     total_control - control_mask.count());
    bool direction =
        (static_cast<double>(test_mask.count())) / (total_test + 1) >
        (static_cast<double>(control_mask.count())) / (total_control + 1);
    out.push_back({anno->data.name, stat, direction});
  }
  return std::tuple<std::string, std::vector<test_result>>(test_name, out);
}

template <typename S, typename D, decltype(fisher_t) fn>
std::tuple<std::string, std::vector<test_result>>
ab_test(const S &test_set, const D &dataset, std::string test_name = "fisher") {
  std::vector<test_result> out;
  // 1, get all hot annotations from test set
  auto hot_mask = test_set.get_mapped_mask();
  // 2, for each annotation
  for (unsigned i = 0; i < hot_mask.size(); ++i) {
    if (!hot_mask.test(i))
      continue;
    // 3, build contingency table
    const auto anno = dataset.get_anno(i);
    auto total_mask = anno->get_mask();
    decltype(total_mask) test_mask = test_set.get_mask(),
                         control_mask = total_mask;
    unsigned total_test = test_mask.count(),
             total_control = dataset.total_syms();
    test_mask &= total_mask;
    // no 4, add test statistics by calling fn
    double stat = fn(test_mask.count(), control_mask.count(),
                     total_test - test_mask.count(),
                     total_control - control_mask.count());
    bool direction =
        (static_cast<double>(test_mask.count())) / (total_test + 1) >
        (static_cast<double>(control_mask.count())) / (total_control + 1);
    out.push_back({anno->data.name, stat, direction});
  }
  return std::tuple<std::string, std::vector<test_result>>(test_name, out);
}
#endif