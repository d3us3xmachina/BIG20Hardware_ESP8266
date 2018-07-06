from PIL import Image, ImageSequence
import struct
import sys
import os
import numpy
import time
from itertools import groupby


filename = raw_input('Please choose source filename:  ')

file_in = filename +".gif"
im = Image.open(file_in)

file_out = open(filename+".bvf", 'wb')
#readable = open(filename+".txt", 'w+')

def pixelColor(Frame, xValue, yValue):
	im.seek(Frame)
	frameImage = im.convert("RGB").load()

	rgb = frameImage[xValue, yValue]

	red = rgb[0]
	green = rgb[1]
	blue = rgb[2]

	Uint16_val = (((31*(red+4))/255)<<11) | (((63*(green+2))/255)<<5) | ((31*(blue+4))/255)
	hex_16 = int('%.4x'%Uint16_val, 16)
	return hex_16


for i, page in enumerate(ImageSequence.Iterator(im)):
	for y in range (0,128):
		for x in range (0,128):
			currentPixel = pixelColor(i,x,y)
			file_out.write(struct.pack('>1H', currentPixel))