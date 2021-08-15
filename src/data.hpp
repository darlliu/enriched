#ifndef DATA
#define DATA
#include <bitset>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace enriched {};
using namespace enriched;

struct _annotation {
  const std::string id, name, description;
};

struct _symbol {
  const std::string sym, name;
};

template <typename dtype, size_t BITSIZE> struct _datum {
  const std::shared_ptr<dtype> data;
  std::bitset<BITSIZE> mappings;
};

typedef _datum<_annotation, 8> annotation8;
typedef _datum<_annotation, 16> annotation16;
typedef _datum<_annotation, 20> annotation20;
typedef _datum<_annotation, 24> annotation24;
typedef _datum<_annotation, 28> annotation28;
typedef _datum<_annotation, 32> annotation32;
typedef _datum<_symbol, 8> symbol8;
typedef _datum<_symbol, 16> symbol16;
typedef _datum<_symbol, 20> symbol20;
typedef _datum<_symbol, 24> symbol24;
typedef _datum<_symbol, 28> symbol28;
typedef _datum<_symbol, 32> symbol32;

// dataset class is a total annotated dataset consisting of
// symbols and their associated annotations
// it provides simple method such as decoding symbols, finding associations etc
template <typename stype, typename atype> class Dataset {
private:
  std::vector<stype> syms;
  std::vector<atype> annos;
  std::unordered_map<std::string, unsigned> _seen_annos, _seen_syms;

public:
  const std::vector<atype *>
  decode_annos(const decltype(stype::mappings) &encoding) {
    std::vector<atype *> out;
    out.reserve(encoding.size());
    for (unsigned i = 0; i < encoding.size(); ++i) {
      if (encoding.test(i)) {
        out.push_back(&annos[i]);
      }
    }
    return out;
  };
  const std::vector<stype *>
  decode_syms(const decltype(atype::mappings) &encoding) {
    std::vector<stype *> out;
    out.reserve(encoding.size());
    for (unsigned i = 0; i < encoding.size(); ++i) {
      if (encoding.test(i)) {
        out.push_back(&syms[i]);
      }
    }
    return out;
  };
  void add_sym(const std::string &&sym, const std::string &&name,
               const std::vector<std::string> &&mapped = {}) {
    if (_seen_syms.count(sym)) {
      return;
    } else {
      _seen_syms[sym] = syms.size();
    }
    decltype(stype::mappings) mappings = 0;
    if (mapped.size() > 0) {
      for (auto anno : mapped) {
        mappings.set(_seen_annos[anno]);
      }
    }
    syms.push_back(
        stype{std::make_shared<_symbol>(_symbol{sym, name}), mappings});
    return;
  };
  void add_anno(const std::string &&id, const std::string &&name,
                const std::string &&desc,
                const std::vector<std::string> &&mapped = {}) {
    if (_seen_annos.count(id)) {
      return;
    } else {
      _seen_annos[id] = annos.size();
    }
    decltype(atype::mappings) mappings = 0;
    if (mapped.size() > 0) {
      for (auto sym : mapped) {
        mappings.set(_seen_syms[sym]);
      }
    }
    annos.push_back(atype{
        std::make_shared<_annotation>(_annotation{id, name, desc}), mappings});
    return;
  };
  void gen_mappings() {
    if (syms.size() == 0 || annos.size() == 0) {
      return;
    }

    decltype(stype::mappings) bitmask_sym;
    for (const auto &sym : syms) {
      bitmask_sym |= sym.mappings;
    }

    decltype(atype::mappings) bitmask_anno;
    for (const auto &anno : annos) {
      bitmask_anno |= anno.mappings;
    }

    if (bitmask_sym.none()) {
      for (const auto &anno : annos) {
        for (int i = 0; i < syms.size(); ++i) {
          syms[i].mappings[_seen_annos.at(anno.data->id)] = anno.mappings[i];
        }
      }
    } else if (bitmask_anno.none()) {
      for (const auto &sym : syms) {
        for (int i = 0; i < annos.size(); ++i) {
          annos[i].mappings[_seen_syms.at(sym.data->sym)] = sym.mappings[i];
        }
      }
    }
    return;
  };
  decltype(atype::mappings)
  encode_syms(const std::vector<std::string> &&mapped) {
    decltype(atype::mappings) out = 0;
    for (auto sym : mapped) {
      if (_seen_syms.count(sym))
        out.set(_seen_syms.at(sym));
    }
    return out;
  };
  decltype(stype::mappings)
  encode_annos(const std::vector<std::string> &&mapped) {
    decltype(stype::mappings) out = 0;
    for (auto anno : mapped) {
      if (_seen_annos.count(anno))
        out.set(_seen_annos.at(anno));
    }
    return out;
  };
};

// a Set is a collection of either symbols or annotations that contains a subset
// of those data contains simple methods such as getting all items in the set,
// getting all mappings (union)
template <typename dtype, typename dsettype> class Set {
private:
  std::vector<unsigned> idxs;
  const dsettype *source;

public:
  Set(const std::vector<std::string> &&data, const dsettype &src);
  Set(const std::vector<dtype> &&data, const dsettype &src);
  const std::vector<dtype *> get();
  template <size_t BITSIZE> std::bitset<BITSIZE> get_mask();
  template <size_t BITSIZE> std::bitset<BITSIZE> get_mapped_mask();
};
#endif