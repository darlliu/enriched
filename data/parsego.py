
with open("../resources/go.obo", "r") as f, open("./go.anno.tsv", "w") as g:
    seen = {}
    ids = None
    name = None
    desc = None
    for line in f.readlines():
        if "[Term]" in line:
            ids = None
            name = None
            desc = None
        if len(line) < 2:
            continue
        if "id:" == line[:3] and "GO" in line:
            ids = line[3:].strip()
        elif "name:" == line[:5]:
            name = line[5:].strip()
        elif "def:" == line[:4]:
            desc = line[4:].strip().strip("")
        if ids and name and desc:
            if ids not in seen:
                ss = "{}\t{}\t{}\n".format(ids, name, desc)
                g.write(ss)
                seen[ids] = 1

mappings = {}
with open("../resources/goa_human.gaf", "r") as f, open("./go.sym.tsv", "w") as g:
    sym = None
    terms = []
    for line in f.readlines():
        if line[0] == '!':
            continue
        line = line.split()
        if line[2] not in mappings:
            mappings[line[2]] = [line[4]]
        else:
            mappings[line[2]].append(line[4])
    for key in mappings:
        g.write("{}\t{}\n".format(key, ",".join(mappings[key])))
