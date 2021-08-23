#ifndef EXPORTS
#define EXPORTS
#include "data.hpp"
#include "stats.hpp"
using namespace enriched;
using namespace std;

typedef Dataset<annotation8, symbol8> SmallDataset;
typedef Dataset<annotation16, symbol16> StandardDataset;
typedef Dataset<annotation18, symbol8> BigDataset;
typedef Dataset<annotation24, symbol24> HugeDataset;

typedef SymSet<symbol8, SmallDataset> SmallSymSet;
typedef SymSet<symbol16, StandardDataset> StandardSymSet;
typedef SymSet<symbol18, BigDataset> BigSymSet;
typedef SymSet<symbol24, HugeDataset> HugeSymSet;
typedef AnnoSet<annotation8, SmallDataset> SmallAnnoSet;
typedef AnnoSet<annotation16, StandardDataset> StandardAnnoSet;
typedef AnnoSet<annotation18, BigDataset> BigAnnoSet;
typedef AnnoSet<annotation24, HugeDataset> HugeAnnoSet;

auto standard_fisher = fisher_test<StandardSymSet, StandardDataset>;
auto standard_fisher_ab = fisher_test_ab<StandardSymSet, StandardDataset>;

#endif