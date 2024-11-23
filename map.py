from PIL import Image
from random import random

SIZE = 128
HALFSIZE = int(SIZE/2)
QUARTERSIZE = int(SIZE/4)
THREEQUARTERSIZE = 3*QUARTERSIZE
FACTOR = 2
WATER = (0, 0, 200)
PLAIN = (0, 200, 0)

m = []

for i in range(SIZE+1):
    m.append([])

for i in range(SIZE+1):
     for j in range(SIZE+1):
        m[i].append(None)

m[0][0] = 100
m[0][QUARTERSIZE] = 100
m[0][HALFSIZE] = 100
m[0][THREEQUARTERSIZE] = 100
m[0][SIZE] = 100

m[QUARTERSIZE][0] = 100
m[QUARTERSIZE][QUARTERSIZE] = 150
m[QUARTERSIZE][HALFSIZE] = 100
m[QUARTERSIZE][THREEQUARTERSIZE] = 256
m[QUARTERSIZE][SIZE] = 100

m[HALFSIZE][0] = 100
m[HALFSIZE][QUARTERSIZE] = 100
m[HALFSIZE][HALFSIZE] = 140
m[HALFSIZE][THREEQUARTERSIZE] = 180
m[HALFSIZE][SIZE] = 100

m[THREEQUARTERSIZE][0] = 100
m[THREEQUARTERSIZE][QUARTERSIZE] = 256
m[THREEQUARTERSIZE][HALFSIZE] = 180
m[THREEQUARTERSIZE][THREEQUARTERSIZE] = 180
m[THREEQUARTERSIZE][SIZE] = 100

m[SIZE][0] = 100
m[SIZE][QUARTERSIZE] = 100
m[SIZE][HALFSIZE] = 100
m[SIZE][THREEQUARTERSIZE] = 100
m[SIZE][SIZE] = 100

counter = 0

def draw_map(m, x, y, amplitude):
    global counter
    if amplitude == 1:
        return
    counter += 1
    midamp = int(amplitude/2)
    v = m[x+midamp][y]
    if v is None:
        try:
            m[x+midamp][y] = round( (m[x][y] + m[x+amplitude][y])/2 + amplitude*(random()-0.5)*FACTOR )
        except:
            print(x, y, amplitude)

    v = m[x+midamp][y+amplitude]
    if v is None:
        m[x+midamp][y+amplitude] = round( (m[x][y+amplitude] + m[x+amplitude][y+amplitude])/2 + amplitude*(random()-0.5)*FACTOR )

    v = m[x][y+midamp]
    if v is None:
        m[x][y+midamp] = round( (m[x][y] + m[x][y+amplitude])/2 + amplitude*(random()-0.5)*FACTOR )

    v = m[x+amplitude][y+midamp]
    if v is None:
        m[x+amplitude][y+midamp] = round( (m[x+amplitude][y] + m[x+amplitude][y+amplitude])/2 + amplitude*(random()-0.5)*FACTOR )

    v = m[x+midamp][y+midamp]
    if v is None:
        m[x+midamp][y+midamp] = round( (m[x+midamp][y] + m[x+midamp][y+amplitude] + m[x][y+midamp] + m[x+amplitude][y+midamp])/4 + amplitude*(random()-0.5)*FACTOR )

    draw_map(m, x, y, midamp)
    draw_map(m, x+midamp, y, midamp)
    draw_map(m, x+midamp, y+midamp, midamp)
    draw_map(m, x, y+midamp, midamp)

draw_map(m, 0, 0, HALFSIZE)
draw_map(m, HALFSIZE, 0, HALFSIZE)
draw_map(m, 0, HALFSIZE, HALFSIZE)
draw_map(m, HALFSIZE, HALFSIZE, HALFSIZE)

bitmap = [((x, x, x) if x > 180 else PLAIN) if x > 128 else WATER for xs in m for x in xs]
img = Image.new('RGB', (SIZE+1, SIZE+1))
img.putdata(bitmap)
#img.show()
print(counter)

def water(x, y):
    nb_coast = 0
    if bitmap[x-1 + (y-1)*(SIZE+1)] == PLAIN:
        nb_coast += 1
    if bitmap[x-1 + (y)*(SIZE+1)] == PLAIN:
        nb_coast += 1
    if bitmap[x-1 + (y+1)*(SIZE+1)] == PLAIN:
        nb_coast += 1
    if bitmap[x + (y-1)*(SIZE+1)] == PLAIN:
        nb_coast += 1
    if bitmap[x + (y+1)*(SIZE+1)] == PLAIN:
        nb_coast += 1
    if bitmap[x+1 + (y-1)*(SIZE+1)] == PLAIN:
        nb_coast += 1
    if bitmap[x+1 + (y)*(SIZE+1)] == PLAIN:
        nb_coast += 1
    if bitmap[x+1 + (y+1)*(SIZE+1)] == PLAIN:
        nb_coast += 1

    if nb_coast == 1:
        if bitmap[x-1 + (y-1)*(SIZE+1)] == PLAIN:
            return "27"
        if bitmap[x-1 + (y)*(SIZE+1)] == PLAIN:
            return "26"
        if bitmap[x-1 + (y+1)*(SIZE+1)] == PLAIN:
            return "25"
        if bitmap[x + (y-1)*(SIZE+1)] == PLAIN:
            return "19"
        if bitmap[x + (y+1)*(SIZE+1)] == PLAIN:
            return "17"
        if bitmap[x+1 + (y-1)*(SIZE+1)] == PLAIN:
            return "11"
        if bitmap[x+1 + (y)*(SIZE+1)] == PLAIN:
            return "10"
        if bitmap[x+1 + (y+1)*(SIZE+1)] == PLAIN:
            return "9"

    if nb_coast == 2:
        if bitmap[x-1 + (y-1)*(SIZE+1)] == PLAIN and bitmap[x-1 + (y)*(SIZE+1)] == PLAIN:
            return "26"
        if bitmap[x-1 + (y)*(SIZE+1)] == PLAIN and bitmap[x-1 + (y+1)*(SIZE+1)] == PLAIN:
            return "26"
        if bitmap[x+1 + (y-1)*(SIZE+1)] == PLAIN and bitmap[x+1 + (y)*(SIZE+1)] == PLAIN:
            return "10"
        if bitmap[x+1 + (y)*(SIZE+1)] == PLAIN and bitmap[x+1 + (y+1)*(SIZE+1)] == PLAIN:
            return "10"
        if bitmap[x-1 + (y-1)*(SIZE+1)] == PLAIN and bitmap[x + (y-1)*(SIZE+1)] == PLAIN:
            return "19"
        if bitmap[x + (y-1)*(SIZE+1)] == PLAIN and bitmap[x+1 + (y-1)*(SIZE+1)] == PLAIN:
            return "19"
        if bitmap[x-1 + (y+1)*(SIZE+1)] == PLAIN and bitmap[x + (y+1)*(SIZE+1)] == PLAIN:
            return "17"
        if bitmap[x + (y+1)*(SIZE+1)] == PLAIN and bitmap[x+1 + (y+1)*(SIZE+1)] == PLAIN:
            return "17"

    if nb_coast == 3:
        if bitmap[x-1 + (y-1)*(SIZE+1)] == PLAIN and bitmap[x-1 + (y)*(SIZE+1)] == PLAIN and bitmap[x-1 + (y+1)*(SIZE+1)] == PLAIN:
            return "26"
        if bitmap[x+1 + (y-1)*(SIZE+1)] == PLAIN and bitmap[x+1 + (y)*(SIZE+1)] == PLAIN and bitmap[x+1 + (y+1)*(SIZE+1)] == PLAIN:
            return "10"
        if bitmap[x-1 + (y-1)*(SIZE+1)] == PLAIN and bitmap[x + (y-1)*(SIZE+1)] == PLAIN and bitmap[x+1 + (y-1)*(SIZE+1)] == PLAIN:
            return "19"
        if bitmap[x-1 + (y+1)*(SIZE+1)] == PLAIN and bitmap[x + (y+1)*(SIZE+1)] == PLAIN and bitmap[x+1 + (y+1)*(SIZE+1)] == PLAIN:
            return "17"

    if nb_coast == 3 or nb_coast == 4:
        if bitmap[x-1 + (y-1)*(SIZE+1)] == PLAIN and bitmap[x-1 + (y)*(SIZE+1)] == PLAIN and bitmap[x + (y-1)*(SIZE+1)] == PLAIN:
            return "46"
        if bitmap[x + (y+1)*(SIZE+1)] == PLAIN and bitmap[x-1 + (y+1)*(SIZE+1)] == PLAIN and bitmap[x-1 + (y)*(SIZE+1)] == PLAIN:
            return "48"
        if bitmap[x+1 + (y-1)*(SIZE+1)] == PLAIN and bitmap[x+1 + (y)*(SIZE+1)] == PLAIN and bitmap[x + (y-1)*(SIZE+1)] == PLAIN:
            return "62"
        if bitmap[x+1 + (y)*(SIZE+1)] == PLAIN and bitmap[x+1 + (y+1)*(SIZE+1)] == PLAIN and bitmap[x+1 + (y+1)*(SIZE+1)] == PLAIN:
            return "64"

    if nb_coast == 5:
        if bitmap[x+1 + (y-1)*(SIZE+1)] != PLAIN and bitmap[x+1 + (y)*(SIZE+1)] != PLAIN and bitmap[x + (y-1)*(SIZE+1)] != PLAIN:
            return "48"
        if bitmap[x + (y+1)*(SIZE+1)] != PLAIN and bitmap[x-1 + (y+1)*(SIZE+1)] != PLAIN and bitmap[x-1 + (y)*(SIZE+1)] != PLAIN:
            return "62"
        if bitmap[x-1 + (y-1)*(SIZE+1)] != PLAIN and bitmap[x-1 + (y)*(SIZE+1)] != PLAIN and bitmap[x + (y-1)*(SIZE+1)] != PLAIN:
            return "64"
        if bitmap[x+1 + (y)*(SIZE+1)] != PLAIN and bitmap[x+1 + (y+1)*(SIZE+1)] != PLAIN and bitmap[x+1 + (y+1)*(SIZE+1)] != PLAIN:
            return "46"

    return "22" if x % 2 else "23"


with open('auxilia2.tmx', 'wt') as f:
    f.write("""<?xml version="1.0" encoding="UTF-8"?>
<map version="1.10" tiledversion="1.10.2" orientation="orthogonal" renderorder="right-down" width="128" height="128" tilewidth="16" tileheight="16" infinite="0" nextlayerid="2" nextobjectid="1">
 <tileset firstgid="1" source="auxilia.tsx"/>
 <layer id="1" name="Tile Layer 1" width="128" height="128">
  <data encoding="csv">""")
    data = []
    for x in range(SIZE):
        for y in range(SIZE):
            cell = bitmap[x+y*(SIZE+1)]
            if cell == PLAIN:
                data.append("4")
            elif cell == WATER:
                data.append(water(x, y))
            else:
                data.append("14")
    f.write(",".join(data))
    f.write("""</data>
 </layer>
</map>""")
