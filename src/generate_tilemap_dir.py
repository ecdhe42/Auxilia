tilemap = [  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255, 16,  0,255,  0,  0,  0,  0,  0,  0,  0,  0,255,  0, 64,255,
  255,255,  0,255,  0,  8,255,  0,  0,255,  9,  0,255,  0,255,255,
  255,  0,  0,  0,  0,255,255,  0,  0,255,255,  0,  0,  0,  0,255,
  255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,
  255,  0,255,  0,255,255,  0,  0,  0,  0,255,255,  0,255,  0,255,
  255,  0,  0,  0,  0,255,  0,  0,  0,  0,255,  0,  0,  0,  0,255,
  255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,
  255,  0,  0,  0,  0,255,  0,  0,  0,  0,255,  0,  0,  0,  0,255,
  255,  0,255,  0,255,255,  0,  0,  0,  0,255,255,  0,255,  0,255,
  255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,
  255,  0,  0,  0,  0,255,255,  0,  0,255,255,  0,  0,  0,  0,255,
  255,255,  0,255,  0, 10,255,  0,  0,255, 11,  0,255,  0,255,255,
  255,128,  0,255,  0,  0,  0,  0,  0,  0,  0,  0,255,  0, 32,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0]

def get_dir(i):
    if tilemap[i] == 255:
        return 0
    c = 0
    if tilemap[i+1] != 255:
        c += 1
    if tilemap[i-1] != 255:
        c += 2
    if tilemap[i+16] != 255:
        c += 4
    if tilemap[i-16] != 255:
        c += 8
    return c

tilemap_dir = [str(get_dir(i)).rjust(3) for i in range(240)]
for i in range(15):
    print(','.join(tilemap_dir[i*16:i*16+16]) + ('' if i == 14 else ','))
