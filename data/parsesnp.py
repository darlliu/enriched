mappings = {}
annos = {}
with open("../resources/clinvar.tsv", "r") as f, open("snp.anno.tsv", "w") as g, open("rs.tsv", "w") as h:
    for line in f.readlines():
        if '#' == line[0]:
            continue
        line = line.split()
        ref = line[3]
        alt = line[4]
        line = line[-1]
        line = line.split(";")
        rs = None
        anno = None
        desc = None
        gene = None
        for token in line:
            if token.startswith("RS="):
                rs = token[3:]
            elif token.startswith("CLNDN="):
                desc = token[6:]
            elif token.startswith("CLNDISDB="):
                anno = token[9:]
            elif token.startswith("GENEINFO="):
                gene = token[9:]
        if rs not in mappings:
            mappings[rs] = (ref, alt, gene)
        if gene not in annos:
            annos[gene] = (desc, anno)
    for key in annos:
        g.write("{}\t{}\t{}\n".format(key, *annos[key]))
    for key in mappings:
        h.write("{}\t{}\t{}\t{}\n".format(key, *mappings[key]))


with open("../resources/snp.txt", "r") as f, open("snp.sig.tsv", "w") as g, open("snp.all.tsv", "w") as h:
    for line in f.readlines():
        if line[0] == '#':
            continue
        line = line.split()
        gt = line[-1]
        rs = line[0][2:]
        if rs not in mappings:
            continue
        alt = mappings[rs][1]
        if alt == gt or alt in gt:
            g.write("{}\n".format(rs))
        h.write("{}\t{}\n".format(rs, mappings[rs][-1]))
