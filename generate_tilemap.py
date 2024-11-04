import xml.etree.ElementTree as ET 

tree1 = ET.parse("auxilia.tmx")
root1 = tree1.getroot()
tree2 = ET.parse("auxilia_cities.tmx")
root2 = tree2.getroot()
tree3 = ET.parse("dungeon.tmx")
root3 = tree3.getroot()

tiles_world = [str(int(val)-1) for val in root1.findall('./layer/data')[0].text.split(',')]
tiles_cities = [str(int(val)-1) for val in root2.findall('./layer/data')[0].text.split(',')]
tiles_dungeon = [str(int(val)-1) for val in root3.findall('./layer/data')[0].text.split(',')]

def export(f, tiles, idx):
    block = tiles
    f.write('#pragma rodata-name(push, "PROG' + str(idx + 2) +'")\n')
    f.write('const unsigned char tilemap' + str(idx) + '[16384] = {\n')
    for i in range(64):
        row = [str(v) for v in block[i*256:i*256+256]]
        f.write('  ' + ','.join(row) + ('\n' if i == 63 else ',\n'))
    f.write('};\n')
    f.write("#pragma rodata-name (pop)\n")
    f.write("\n")

with open("src/tilemap.h", "w") as f:
    export(f, tiles_world, 0)
    export(f, tiles_cities, 1)
    export(f, tiles_dungeon, 2)

#    export(f, 1)
#    export(f, 2)
#    export(f, 3)
