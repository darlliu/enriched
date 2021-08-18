#ifndef DATA
#define DATA
#include <bitset>
#include <memory>
#include <stdexcept>
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
  const dtype data;
  std::vector<unsigned> mapped;
  typedef std::bitset<BITSIZE> mappings;
  _datum(const dtype &d, const std::vector<unsigned> &m) : data(d), mapped(m){};
  _datum(const dtype &&d, const std::vector<unsigned> &&m)
      : data(std::move(d)), mapped(std::move(m)){};
  constexpr std::unique_ptr<mappings> get_mask() const {
    auto out = std::make_unique<mappings>();
    for (const unsigned &idx : mapped)
      out->set(idx);
    return out;
  };
};

typedef _datum<_annotation, 2 << 8> annotation8;
typedef _datum<_annotation, 2 << 15> annotation15;
typedef _datum<_annotation, 2 << 16> annotation16;
typedef _datum<_annotation, 2 << 18> annotation18;
typedef _datum<_annotation, 2 << 20> annotation20;
typedef _datum<_annotation, 2 << 24> annotation24;
typedef _datum<_annotation, 2 << 28> annotation28;
typedef _datum<_symbol, 2 << 8> symbol8;
typedef _datum<_symbol, 2 << 15> symbol15;
typedef _datum<_symbol, 2 << 16> symbol16;
typedef _datum<_symbol, 2 << 18> symbol18;
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
  constexpr const unsigned total_syms() const { return syms.size(); };
  constexpr const unsigned total_annos() const { return annos.size(); };
  constexpr const unsigned anno_idx(const std::string &anno) const {
    return _seen_annos.at(anno);
  };
  constexpr const unsigned sym_idx(const std::string &sym) const {
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
  constexpr const bool has_anno(const std::string &anno) const {
    return _seen_annos.count(anno);
  };
  constexpr const bool has_sym(const std::string &sym) const {
    return _seen_syms.count(sym);
  };
  const std::vector<const atype *>
  decode_annos(const typename stype::mappings &encoding) const {
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
  decode_syms(const typename atype::mappings &encoding) const {
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
    std::vector<unsigned> mappings;
    if (mapped.size() > 0) {
      for (auto anno : mapped) {
        if (has_anno(anno))
          mappings.push_back(anno_idx(anno));
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
    std::vector<unsigned> mappings;
    if (mapped.size() > 0) {
      for (auto anno : mapped) {
        if (has_anno(anno))
          mappings.push_back(anno_idx(anno));
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
    std::vector<unsigned> mappings;
    if (mapped.size() > 0) {
      for (auto sym : mapped) {
        if (has_sym(sym))
          mappings.push_back(sym_idx(sym));
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
    std::vector<unsigned> mappings;
    if (mapped.size() > 0) {
      for (auto sym : mapped) {
        if (has_sym(sym))
          mappings.push_back(sym_idx(sym));
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
    unsigned sym_count = 0;
    for (const auto &sym : syms) {
      sym_count += sym->mapped.size();
    }
    unsigned anno_count = 0;
    for (const auto &anno : annos) {
      anno_count += anno->mapped.size();
    }
    if (sym_count == 0) {
      for (const auto &anno : annos) {
        for (const unsigned &sidx : anno->mapped) {
          syms[sidx]->mapped.push_back(anno_idx(anno->data.id));
        }
      }
    } else if (anno_count == 0) {
      for (const auto &sym : syms) {
        for (const unsigned &aidx : sym->mapped) {
          annos[aidx]->mapped.push_back(sym_idx(sym->data.sym));
        }
      }
    }
    return;
  };
  std::unique_ptr<typename atype::mappings>
  encode_syms(const std::vector<std::string> &mapped) const {
    auto out = std::make_unique<typename atype::mappings>();
    for (const auto &sym : mapped) {
      if (has_sym(sym))
        out->set(sym_idx(sym));
    }
    return out;
  };
  std::unique_ptr<typename stype::mappings>
  encode_annos(const std::vector<std::string> &mapped) const {
    auto out = std::make_unique<typename stype::mappings>();
    for (const auto &anno : mapped) {
      if (has_anno(anno))
        out->set(anno_idx(anno));
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
  typename dtype::mappings mapped_mask;

public:
  Set(const std::vector<std::string> &data, dsettype &src) {
    idxs.reserve(data.size());
    source = &src;
  };
  virtual const std::vector<const dtype *> get() const = 0;
  std::unique_ptr<typename dtype::mappings> get_mapped_mask() const {
    return std::make_unique<typename dtype::mappings>(mapped_mask);
  };
  void _get_mapped_mask() {
    for (const auto &dt : get()) {
      for (const unsigned &idx : dt->mapped) {
        mapped_mask.set(idx);
      }
    }
  };
};

template <typename stype, typename atype>
class SymSet : public Set<stype, Dataset<stype, atype>> {
private:
  typename atype::mappings mask;

public:
  SymSet(const std::vector<std::string> &data, Dataset<stype, atype> &src)
      : Set<stype, Dataset<stype, atype>>(data, src) {
    for (const auto &sym : data) {
      if (this->source->has_sym(sym)) {
        this->idxs.push_back(this->source->sym_idx(sym));
      }
    }
    _get_mask();
    this->_get_mapped_mask();
  };
  const std::vector<const stype *> get() const {
    std::vector<const stype *> out;
    out.reserve(this->idxs.size());
    for (unsigned idx : this->idxs) {
      out.push_back(this->source->get_sym(idx));
    }
    return out;
  };
  std::unique_ptr<typename atype::mappings> get_mask() const {
    return std::make_unique<typename atype::mappings>(mask);
  };
  void _get_mask() {
    for (auto &idx : this->idxs) {
      mask.set(idx);
    }
  };
};

template <typename stype, typename atype>
class AnnoSet : public Set<atype, Dataset<stype, atype>> {
private:
  typename stype::mappings mask;

public:
  AnnoSet(const std::vector<std::string> &data,
          const Dataset<stype, atype> &src)
      : Set<atype, Dataset<stype, atype>>(data, src) {
    for (const auto &sym : data) {
      if (this->source->has_anno(sym)) {
        this->idxs.push_back(this->source->anno_idx(sym));
      }
    }
    _get_mask();
    this->_get_mapped_mask();
  };
  const std::vector<const atype *> get() const {
    std::vector<const atype *> out;
    for (const auto &idx : this->idxs) {
      out.push_back(this->source->get_anno(idx));
    }
    return out;
  };
  std::unique_ptr<typename stype::mappings> get_mask() const {
    return std::make_unique<typename stype::mappings>(mask);
  };
  void _get_mask() {
    for (auto &idx : this->idxs) {
      mask.set(idx);
    }
  };
};
#endif