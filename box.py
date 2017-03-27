# Python file to parse text0.mp4 for boxes, and extract images.

import struct
import sys
import xml.etree.ElementTree as ET
import base64

# Process MDAT Data -> Look for image tag in XML, extract text, base64 decode, and save as png files.
def processMDAT(filehandle, size):
    imgCount = 1;
    xmlString = filehandle.read( size)
    if(xmlString == ""):                # Read Error
        return ""
    root = ET.fromstring(xmlString)     #Create XML Element
    
    for elem in root.iter():            # Iterate through XML, Look for Image Tag
        if 'image' in elem.tag:
            image = base64.b64decode(elem.text) #Base 64 decode images
            image_bin = bytearray(image)    #Convert to binary
            outfilename = "image"+str(imgCount)+".png" #Get a Filename
            outfile = open(outfilename, "wb")
            outfile.write(image_bin)                #Write File
            outfile.close()
            imgCount = imgCount +1
    return 1
            
def readBinary4(filehandle):
    numString = filehandle.read(4)
    
    if numString == "":
        return ""
    numTuple = struct.unpack('>I',numString)
    
    return int(numTuple[0])
    
def processBox(filehandle):
    boxSize = readBinary4(filehandle)
    if boxSize == "":
        return ""
    boxType = filehandle.read(4)
    if boxType == "":
        return ""
    
    print "Found box of type %s and size %d\n" %(boxType, boxSize)
    
    # MOOF and TRAF Files have boxes in them
    if ((boxType == "moof") or (boxType == "traf")):
        # These boxes have children
        return processBox(filehandle)
    # Process mdat data
    if (boxType == "mdat"):
        return processMDAT(filehandle, boxSize -8) # Account for 8 byte header
    
    # In all other cases, just skip the payload
    if(filehandle.seek(boxSize -8,1) == ""):
        return ""
    return 1

if __name__ == "__main__":
    filehandle =open(sys.argv[1],"rb")
    
    while(processBox(filehandle) != ""):
        continue
    filehandle.close()