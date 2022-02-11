import sys
dic = {}
poly = {}
with open(sys.argv[1], 'r', encoding = 'big5-hkscs') as f:
    line = f.readline()
    while len(line) != 0:
        test_poly = line[2]
        for i in line[2:-1].split('/'):
            if i[0] not in dic:
                dic[i[0]] = []
            if line[0] not in dic[i[0]]:
                dic[i[0]].append(line[0])
            if test_poly != i[0]:
                poly[line[0]] = False
        line = f.readline()

with open(sys.argv[2], 'w', encoding = 'big5-hkscs') as f:
    for i in dic:
        line = i+'  '
        for j in dic[i]:
            line += j+' '
        f.write(line[:-1]+'\n')
        for j in dic[i]:
            if j in poly:
                if poly[j] == False:
                    poly[j] = True
                else:
                    continue
            f.write(j+'  '+j+'\n')
