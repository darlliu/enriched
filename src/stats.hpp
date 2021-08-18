#ifndef STATS
#define STATS
#include <algorithm>
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

constexpr double fold_change(const unsigned &_a, const unsigned &_b,
                             const unsigned &_c, const unsigned &_d) {
  double ans = 1.0;
  if (_a + _c == 0 || _b + _d == 0)
    return ans;
  double a = static_cast<double>(_a), b = static_cast<double>(_b),
         c = static_cast<double>(_c), d = static_cast<double>(_d);
  double r1 = a / (a + c);
  double r2 = b / (b + d);
  if (r2 == 0)
    return ans;
  return r1 / r2;
}

struct test_result {
  std::string name;
  double stat = -1000;
  bool enriched = false;
};

constexpr bool stat_sig_05(const test_result &res) { return res.stat > 0.05; }
constexpr bool stat_sig_01(const test_result &res) { return res.stat > 0.01; }
constexpr bool stat_sig_005(const test_result &res) { return res.stat > 0.005; }
constexpr bool fold_1(const test_result &res) { return res.stat <= 1.0; }

constexpr bool ascending(const test_result &a, const test_result &b) {
  return a.stat < b.stat;
}
constexpr bool descending(const test_result &a, const test_result &b) {
  return a.stat > b.stat;
}

template <typename S, typename D, decltype(fisher_t) fn, decltype(fold_1) gn,
          decltype(ascending) cmp>
std::tuple<std::string, std::vector<test_result>>
ab_test(const S &test_set, const S &control_set, const D &dataset,
        std::string test_name = "fisher") {
  std::vector<test_result> out;
  // 1, get all hot annotations from test set
  auto hot_mask = test_set.get_mapped_mask();
  auto test_mask = test_set.get_mask(), control_mask = control_set.get_mask();
  unsigned total_test = test_mask->count(),
           total_control = control_mask->count();
  // 2, for each annotation
  for (unsigned i = 0; i < hot_mask->size(); ++i) {
    if (!hot_mask->test(i))
      continue;
    // 3, build contingency table
    const auto anno = dataset.get_anno(i);
    auto total_mask = anno->get_mask();
    test_mask = test_set.get_mask(), control_mask = control_set.get_mask();
    test_mask->operator&=(*total_mask);
    control_mask->operator&=(*total_mask);
    unsigned test_count = test_mask->count(),
             control_count = control_mask->count();
    // no 4, add test statistics by calling fn
    double stat = fn(test_count, control_count, total_test - test_count,
                     total_control - control_count);
    bool enriched = (static_cast<double>(test_count)) /
                        (static_cast<double>(total_test) + 1) >
                    (static_cast<double>(control_count)) /
                        (static_cast<double>(total_control) + 1);
    out.push_back({anno->data.name, stat, enriched});
  }
  out.erase(std::remove_if(out.begin(), out.end(), gn), out.end());
  std::sort(out.begin(), out.end(), cmp);
  out.resize(std::min(out.size(), (size_t)1000));
  return std::tuple<std::string, std::vector<test_result>>(test_name, out);
}

template <typename S, typename D, decltype(fisher_t) fn, decltype(fold_1) gn,
          decltype(ascending) cmp>
std::tuple<std::string, std::vector<test_result>>
ab_test(const S &test_set, const D &dataset, std::string test_name = "fisher") {
  std::vector<test_result> out;
  // 1, get all hot annotations from test set
  auto hot_mask = test_set.get_mapped_mask();
  auto test_mask = test_set.get_mask();
  unsigned total_test = test_mask->count(),
           total_control = dataset.total_syms();
  // 2, for each annotation, generally speaking if the annotation is not
  // in test set, we are not interested either way
  for (unsigned i = 0; i < hot_mask->size(); ++i) {
    if (!hot_mask->test(i))
      continue;
    // 3, build contingency table
    const auto anno = dataset.get_anno(i);
    test_mask = test_set.get_mask();
    auto total_mask = anno->get_mask();
    test_mask->operator&=(*total_mask);
    unsigned test_count = test_mask->count(), total_count = total_mask->count();
    // no 4, add test statistics by calling fn
    double stat = fn(test_count, total_count, total_test - test_count,
                     total_control - test_count);
    bool enriched = (static_cast<double>(test_count)) /
                        (static_cast<double>(total_test) + 1) >
                    (static_cast<double>(total_count)) /
                        (static_cast<double>(total_control) + 1);
    out.push_back({anno->data.name, stat, enriched});
  }
  out.erase(std::remove_if(out.begin(), out.end(), gn), out.end());
  std::sort(out.begin(), out.end(), cmp);
  out.resize(std::min(out.size(), (size_t)1000));
  return std::tuple<std::string, std::vector<test_result>>(test_name, out);
}

template <typename S, typename D, decltype(fisher_t) fn, decltype(fold_1) gn,
          decltype(ascending) cmp>
std::tuple<std::string, std::vector<test_result>>
ab_test_full(const S &test_set, const D &dataset,
             std::string test_name = "fisher") {
  std::vector<test_result> out;
  // 1, get all hot annotations from test set
  auto hot_mask = test_set.get_mapped_mask();
  auto test_mask = test_set.get_mask();
  unsigned total_test = test_mask->count(),
           total_control = dataset.total_syms();
  // 2, for each annotation in all possible annotations (to find negatively
  // enriched)
  for (unsigned i = 0; i < dataset.total_annos(); ++i) {
    // 3, build contingency table
    const auto anno = dataset.get_anno(i);
    test_mask = test_set.get_mask();
    auto total_mask = anno->get_mask();
    test_mask->operator&=(*total_mask);
    unsigned test_count = test_mask->count(), total_count = total_mask->count();
    // no 4, add test statistics by calling fn
    double stat = fn(test_count, total_count, total_test - test_count,
                     total_control - test_count);
    bool enriched = (static_cast<double>(test_count)) /
                        (static_cast<double>(total_test) + 1) >
                    (static_cast<double>(total_count)) /
                        (static_cast<double>(total_control) + 1);
    out.push_back({anno->data.name, stat, enriched});
  }
  out.erase(std::remove_if(out.begin(), out.end(), gn), out.end());
  std::sort(out.begin(), out.end(), cmp);
  out.resize(std::min(out.size(), (size_t)1000));
  return std::tuple<std::string, std::vector<test_result>>(test_name, out);
}

template <typename S, typename D>
auto fisher_test(const S &test_set, const D &dataset) {
  return ab_test<S, D, fisher_t, stat_sig_05, ascending>(
      test_set, dataset, "Fisher's Exact Test (P <= 0.05)");
}

template <typename S, typename D>
auto fisher_test(const S &test_set, const S &control_set, const D &dataset) {
  return ab_test<S, D, fisher_t, stat_sig_05, ascending>(
      test_set, control_set, dataset, "Fisher's Exact Test (P <= 0.05)");
}

template <typename S, typename D>
auto fold_change_test(const S &test_set, const D &dataset) {
  return ab_test<S, D, fold_change, fold_1, descending>(
      test_set, dataset, "Fold Change (Fold > 1)");
}

template <typename S, typename D>
auto fold_change_test(const S &test_set, const S &control_set,
                      const D &dataset) {
  return ab_test<S, D, fold_change, fold_1, descending>(
      test_set, control_set, dataset, "Fold Change (Fold > 1)");
}
#endif