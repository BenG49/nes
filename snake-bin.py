s = '''20 06 06 20 38 06 20 0d 06 20 2a 06 60 a9 02 85 
02 a9 04 85 03 a9 11 85 10 a9 10 85 12 a9 0f 85 
14 a9 04 85 11 85 13 85 15 60 a5 fe 85 00 a5 fe 
29 03 18 69 02 85 01 60 20 4d 06 20 8d 06 20 c3 
06 20 19 07 20 20 07 20 2d 07 4c 38 06 a5 ff c9 
77 f0 0d c9 64 f0 14 c9 73 f0 1b c9 61 f0 22 60 
a9 04 24 02 d0 26 a9 01 85 02 60 a9 08 24 02 d0 
1b a9 02 85 02 60 a9 01 24 02 d0 10 a9 04 85 02 
60 a9 02 24 02 d0 05 a9 08 85 02 60 60 20 94 06 
20 a8 06 60 a5 00 c5 10 d0 0d a5 01 c5 11 d0 07 
e6 03 e6 03 20 2a 06 60 a2 02 b5 10 c5 10 d0 06 
b5 11 c5 11 f0 09 e8 e8 e4 03 f0 06 4c aa 06 4c 
35 07 60 a6 03 ca 8a b5 10 95 12 ca 10 f9 a5 02 
4a b0 09 4a b0 19 4a b0 1f 4a b0 2f a5 10 38 e9 
20 85 10 90 01 60 c6 11 a9 01 c5 11 f0 28 60 e6 
10 a9 1f 24 10 f0 1f 60 a5 10 18 69 20 85 10 b0 
01 60 e6 11 a9 06 c5 11 f0 0c 60 c6 10 a5 10 29 
1f c9 1f f0 01 60 4c 35 07 a0 00 a5 fe 91 00 60 
a6 03 a9 00 81 10 a2 00 a9 01 81 10 60 a2 00 ea 
ea ca d0 fb 60'''

arr = [
    0x20, 0x06, 0x06, 0x20, 0x38, 0x06, 0x20, 0x0d, 0x06, 0x20, 0x2a, 0x06, 0x60, 0xa9, 0x02, 0x85,
    0x02, 0xa9, 0x04, 0x85, 0x03, 0xa9, 0x11, 0x85, 0x10, 0xa9, 0x10, 0x85, 0x12, 0xa9, 0x0f, 0x85,
    0x14, 0xa9, 0x04, 0x85, 0x11, 0x85, 0x13, 0x85, 0x15, 0x60, 0xa5, 0xfe, 0x85, 0x00, 0xa5, 0xfe,
    0x29, 0x03, 0x18, 0x69, 0x02, 0x85, 0x01, 0x60, 0x20, 0x4d, 0x06, 0x20, 0x8d, 0x06, 0x20, 0xc3,
    0x06, 0x20, 0x19, 0x07, 0x20, 0x20, 0x07, 0x20, 0x2d, 0x07, 0x4c, 0x38, 0x06, 0xa5, 0xff, 0xc9,
    0x77, 0xf0, 0x0d, 0xc9, 0x64, 0xf0, 0x14, 0xc9, 0x73, 0xf0, 0x1b, 0xc9, 0x61, 0xf0, 0x22, 0x60,
    0xa9, 0x04, 0x24, 0x02, 0xd0, 0x26, 0xa9, 0x01, 0x85, 0x02, 0x60, 0xa9, 0x08, 0x24, 0x02, 0xd0,
    0x1b, 0xa9, 0x02, 0x85, 0x02, 0x60, 0xa9, 0x01, 0x24, 0x02, 0xd0, 0x10, 0xa9, 0x04, 0x85, 0x02,
    0x60, 0xa9, 0x02, 0x24, 0x02, 0xd0, 0x05, 0xa9, 0x08, 0x85, 0x02, 0x60, 0x60, 0x20, 0x94, 0x06,
    0x20, 0xa8, 0x06, 0x60, 0xa5, 0x00, 0xc5, 0x10, 0xd0, 0x0d, 0xa5, 0x01, 0xc5, 0x11, 0xd0, 0x07,
    0xe6, 0x03, 0xe6, 0x03, 0x20, 0x2a, 0x06, 0x60, 0xa2, 0x02, 0xb5, 0x10, 0xc5, 0x10, 0xd0, 0x06,
    0xb5, 0x11, 0xc5, 0x11, 0xf0, 0x09, 0xe8, 0xe8, 0xe4, 0x03, 0xf0, 0x06, 0x4c, 0xaa, 0x06, 0x4c,
    0x35, 0x07, 0x60, 0xa6, 0x03, 0xca, 0x8a, 0xb5, 0x10, 0x95, 0x12, 0xca, 0x10, 0xf9, 0xa5, 0x02,
    0x4a, 0xb0, 0x09, 0x4a, 0xb0, 0x19, 0x4a, 0xb0, 0x1f, 0x4a, 0xb0, 0x2f, 0xa5, 0x10, 0x38, 0xe9,
    0x20, 0x85, 0x10, 0x90, 0x01, 0x60, 0xc6, 0x11, 0xa9, 0x01, 0xc5, 0x11, 0xf0, 0x28, 0x60, 0xe6,
    0x10, 0xa9, 0x1f, 0x24, 0x10, 0xf0, 0x1f, 0x60, 0xa5, 0x10, 0x18, 0x69, 0x20, 0x85, 0x10, 0xb0,
    0x01, 0x60, 0xe6, 0x11, 0xa9, 0x06, 0xc5, 0x11, 0xf0, 0x0c, 0x60, 0xc6, 0x10, 0xa5, 0x10, 0x29,
    0x1f, 0xc9, 0x1f, 0xf0, 0x01, 0x60, 0x4c, 0x35, 0x07, 0xa0, 0x00, 0xa5, 0xfe, 0x91, 0x00, 0x60,
    0xa6, 0x03, 0xa9, 0x00, 0x81, 0x10, 0xa2, 0x00, 0xa9, 0x01, 0x81, 0x10, 0x60, 0xa2, 0x00, 0xea,
    0xea, 0xca, 0xd0, 0xfb, 0x60
]

with open('snake.bin', 'wb') as f:
	for n in arr:
		f.write(n.to_bytes(1, byteorder='big'))