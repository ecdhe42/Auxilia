from PIL import Image
import xml.etree.ElementTree as ET

tree = ET.parse("shotgun.tmx")
root = tree.getroot()

tileset = Image.open("assets/shotgun/tiles.bmp")

tilemap = [str(int(val)-1) for val in root.findall('./layer/data')[0].text.split(',')]

bitmap = tileset.copy()

obstacles = [['  0']*16, ['  0']*16, ['  0']*16, ['  0']*16,
             ['  0']*16, ['  0']*16, ['  0']*16, ['  0']*16,
             ['  0']*16, ['  0']*16, ['  0']*16, ['  0']*16,
             ['  0']*16, ['  0']*16, ['  0']*16, ['  0']*16]

obstacle_id = 8

for y in range(16):
    for x in range(16):
        idx = int(tilemap[y*16+x])
        # Floor
        if idx == 0:
            pass
        # Gun
        elif idx == 7:
            idx = 0
            obstacles[y][x] = ' '*(3 - len(str(obstacle_id))) + str(obstacle_id)
            obstacle_id += 1
        # Black (bottom bar)
        elif idx > 7:
            obstacles[y][x] = '  0'
        else:
            obstacles[y][x] = '255'
#        else:
#            print(idx, idx == 5)
        if idx >= 16:
            for ty in range(8):
                for tx in range(8):
                    val = tileset.getpixel(((idx-16)*8+tx, ty+8))
                    if val == 0:
                        val = 248
                    bitmap.putpixel((x*8+tx, y*8+ty-1), val)
        else:
            for ty in range(8):
                for tx in range(8):
                    val = tileset.getpixel((idx*8+tx, ty))
                    bitmap.putpixel((x*8+tx, y*8+ty), val)

bitmap.save("assets/shotgun/tilemap.bmp")

for row in obstacles:
    print("  " + ",".join(row) + ",")
