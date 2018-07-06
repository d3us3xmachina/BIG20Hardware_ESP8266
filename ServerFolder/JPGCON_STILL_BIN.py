import struct
from PIL import Image
import numpy
import time
from itertools import groupby

filename = raw_input('Please choose source filename:  ')

file_in = filename +".jpg"
im = Image.open(file_in)

file_out = open(filename+".bif", 'wb')
#readable = open(filename+".txt", 'w+')

pixel_array = []

total_pixels = []

def listsum(numList):
    theSum = 0
    for i in numList:
        theSum = theSum + i
    return theSum


def pixelColor(xValue, yValue):
    frameImage = im.convert("RGB").load()
    rgb = frameImage[xValue, yValue]
    red = rgb[0]
    green = rgb[1]
    blue = rgb[2]

    Uint16_val = (((31*(red+4))/255)<<11) | (((63*(green+2))/255)<<5) | ((31*(blue+4))/255)
    hex_16 = int('%.4x'%Uint16_val, 16)
    return hex_16


for y in range (0,128):
    for x in range (0,128):
        currentPixel = pixelColor(x,y)
        file_out.write(struct.pack('>1H', currentPixel))