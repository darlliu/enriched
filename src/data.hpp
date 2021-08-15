#ifndef DATA
#define DATA
#include <bitset>
#include <exception>
#include <memory>
#include <string>
#include <type_traits>
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
  const dtype data;
  std::bitset<BITSIZE> mappings;
  _datum(const dtype &d, const std::bitset<BITSIZE> &m)
      : data(d), mappings(m){};
  _datum(const dtype &&d, const std::bitset<BITSIZE> &&m)
      : data(std::move(d)), mappings(std::move(m)){};
};

typedef _datum<_annotation, 2 << 8> annotation8;
typedef _datum<_annotation, 2 << 16> annotation16;
typedef _datum<_annotation, 2 << 20> annotation20;
typedef _datum<_annotation, 2 << 24> annotation24;
typedef _datum<_annotation, 2 << 28> annotation28;
typedef _datum<_symbol, 2 << 8> symbol8;
typedef _datum<_symbol, 2 << 16> symbol16;
typedef _datum<_symbol, 2 << 20> symbol20;
typedef _datum<_symbol, 2 << 24> symbol24;
typedef _datum<_symbol, 2 << 28> symbol28;

// dataset class is a total annotated dataset consisting of
// symbols and their associated annotations
// it provides simple method such as decoding symbols, finding associations etc
template <typename stype, typename atype> class Dataset {
private:
  std::vector<std::unique_ptr<stype>> syms;
  std::vector<std::unique_ptr<atype>> annos;
  std::unordered_map<std::string, unsigned> _seen_annos, _seen_syms;

public:
  constexpr const unsigned anno_idx(const std::string &anno) {
    return _seen_annos.at(anno);
  };
  constexpr const unsigned sym_idx(const std::string &sym) {
    return _seen_syms.at(sym);
  };
  constexpr const atype *get_anno(const unsigned &idx) const {
    if (annos.size() > idx) {
      return annos[idx].get();
    } else {
      throw(new std::invalid_argument("invalid index"));
    }
  };
  constexpr const stype *get_sym(const unsigned &idx) const {
    if (syms.size() > idx) {
      return syms[idx].get();
    } else {
      throw(new std::invalid_argument("invalid index"));
    }
  };
  constexpr const atype *get_anno(const std::string &anno) const {
    if (_seen_annos.count(anno)) {
      return annos[_seen_annos.at(anno)].get();
    } else {
      throw(
          new std::invalid_argument("cannot get invalid annotation: " + anno));
    }
  };
  constexpr const stype *get_sym(const std::string &sym) const {
    if (_seen_syms.count(sym)) {
      return syms[_seen_syms.at(sym)].get();
    } else {
      throw(new std::invalid_argument("cannot get invalid symbol: " + sym));
    }
  };
  constexpr const bool has_anno(const std::string &anno) {
    return _seen_annos.count(anno);
  };
  constexpr const bool has_sym(const std::string &sym) {
    return _seen_syms.count(sym);
  };
  const std::vector<const atype *>
  decode_annos(const decltype(stype::mappings) &encoding) {
    std::vector<const atype *> out;
    out.reserve(encoding.size());
    for (unsigned i = 0; i < encoding.size(); ++i) {
      if (encoding.test(i)) {
        out.push_back(get_anno(i));
      }
    }
    return out;
  };
  const std::vector<const stype *>
  decode_syms(const decltype(atype::mappings) &encoding) {
    std::vector<const stype *> out;
    out.reserve(encoding.size());
    for (unsigned i = 0; i < encoding.size(); ++i) {
      if (encoding.test(i)) {
        out.push_back(get_sym(i));
      }
    }
    return out;
  };
  void add_sym(const std::string &sym, const std::string &name,
               const std::vector<std::string> &mapped = {}) {
    if (has_sym(sym)) {
      return;
    } else {
      _seen_syms[sym] = syms.size();
    }
    decltype(stype::mappings) mappings = 0;
    if (mapped.size() > 0) {
      for (auto anno : mapped) {
        mappings.set(anno_idx(anno));
      }
    }
    _symbol data = {sym, name};
    syms.push_back(std::make_unique<stype>(data, mappings));
    return;
  };
  void add_sym(const std::string &&sym, const std::string &&name,
               const std::vector<std::string> &&mapped = {}) {
    if (has_sym(sym)) {
      return;
    } else {
      _seen_syms[sym] = syms.size();
    }
    decltype(stype::mappings) mappings = 0;
    if (mapped.size() > 0) {
      for (auto anno : mapped) {
        mappings.set(anno_idx(anno));
      }
    }
    _symbol data = {std::move(sym), std::move(name)};
    syms.push_back(
        std::make_unique<stype>(std::move(data), std::move(mappings)));
    return;
  };
  void add_anno(const std::string &id, const std::string &name,
                const std::string &desc,
                const std::vector<std::string> &mapped = {}) {
    if (has_anno(id)) {
      return;
    } else {
      _seen_annos[id] = annos.size();
    }
    decltype(atype::mappings) mappings = 0;
    if (mapped.size() > 0) {
      for (auto sym : mapped) {
        mappings.set(sym_idx(sym));
      }
    }
    _annotation data = {id, name, desc};
    annos.push_back(std::make_unique<atype>(data, mappings));
    return;
  };
  void add_anno(const std::string &&id, const std::string &&name,
                const std::string &&desc,
                const std::vector<std::string> &&mapped = {}) {
    if (has_anno(id)) {
      return;
    } else {
      _seen_annos[id] = annos.size();
    }
    decltype(atype::mappings) mappings = 0;
    if (mapped.size() > 0) {
      for (auto sym : mapped) {
        mappings.set(sym_idx(sym));
      }
    }
    _annotation data = {std::move(id), std::move(name), std::move(desc)};
    annos.push_back(
        std::make_unique<atype>(std::move(data), std::move(mappings)));
    return;
  };
  void gen_mappings() {
    if (syms.size() == 0 || annos.size() == 0) {
      return;
    }
    decltype(stype::mappings) bitmask_sym;
    for (const auto &sym : syms) {
      bitmask_sym |= sym->mappings;
    }
    decltype(atype::mappings) bitmask_anno;
    for (const auto &anno : annos) {
      bitmask_anno |= anno->mappings;
    }
    if (bitmask_sym.none()) {
      for (const auto &anno : annos) {
        for (int i = 0; i < syms.size(); ++i) {
          syms[i]->mappings[anno_idx(anno->data.id)] = anno->mappings[i];
        }
      }
    } else if (bitmask_anno.none()) {
      for (const auto &sym : syms) {
        for (int i = 0; i < annos.size(); ++i) {
          annos[i]->mappings[sym_idx(sym->data.sym)] = sym->mappings[i];
        }
      }
    }
    return;
  };
  decltype(atype::mappings)
  encode_syms(const std::vector<std::string> &mapped) {
    decltype(atype::mappings) out = 0;
    for (const auto &sym : mapped) {
      if (has_sym(sym))
        out.set(sym_idx(sym));
    }
    return out;
  };
  decltype(stype::mappings)
  encode_annos(const std::vector<std::string> &mapped) {
    decltype(stype::mappings) out = 0;
    for (const auto &anno : mapped) {
      if (has_anno(anno))
        out.set(anno_idx(anno));
    }
    return out;
  };
};

// a Set is a collection of either symbols or annotations that contains a subset
// of those data contains simple methods such as getting all items in the set,
// getting all mappings (union)
template <typename dtype, typename dsettype> class Set {
protected:
  std::vector<unsigned> idxs;
  dsettype *source;

public:
  Set(const std::vector<std::string> &data, dsettype &src) {
    idxs.reserve(data.size());
    source = &src;
  };
  virtual const std::vector<const dtype *> get() const = 0;
  const decltype(dtype::mappings) get_mapped_mask() const {
    decltype(dtype::mappings) out = 0;
    for (const auto &dt : get()) {
      out |= dt->mappings;
    }
    return out;
  };
};

template <typename stype, typename atype>
class SymSet : public Set<stype, Dataset<stype, atype>> {
public:
  SymSet(const std::vector<std::string> &data, Dataset<stype, atype> &src)
      : Set<stype, Dataset<stype, atype>>(data, src) {
    for (const auto &sym : data) {
      if (this->source->has_sym(sym)) {
        this->idxs.push_back(this->source->sym_idx(sym));
      }
    }
  };
  const std::vector<const stype *> get() const {
    std::vector<const stype *> out;
    out.reserve(this->idxs.size());
    for (unsigned idx : this->idxs) {
      out.push_back(this->source->get_sym(idx));
    }
    return out;
  };
  const decltype(atype::mappings) get_mask() const {
    decltype(atype::mappings) out = 0;
    for (auto &idx : this->idxs) {
      out.set(idx);
    }
    return out;
  }
};

template <typename stype, typename atype>
class AnnoSet : public Set<atype, Dataset<stype, atype>> {
public:
  AnnoSet(const std::vector<std::string> &data,
          const Dataset<stype, atype> &src)
      : Set<atype, Dataset<stype, atype>>(data, src) {
    for (const auto &sym : data) {
      if (this->source->has_anno(sym)) {
        this->idxs.push_back(this->source->anno_idx(sym));
      }
    }
  };
  const std::vector<const atype *> get() const {
    std::vector<const atype *> out;
    for (const auto &idx : this->idxs) {
      out.push_back(this->source->get_anno(idx));
    }
    return out;
  };
  const decltype(stype::mappings) get_mask() const {
    decltype(stype::mappings) out = 0;
    for (auto &idx : this->idxs) {
      out.set(idx);
    }
    return out;
  }
};
#endif