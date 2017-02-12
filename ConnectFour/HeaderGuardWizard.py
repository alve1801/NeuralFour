import os
from os import listdir
from os.path import isfile, join


Headers = []

#UnGuardedParts = ["#"]

def CompleteHeaderName(HeaderName):
    if HeaderName[-2:] != ".h":
        HeaderName += ".h"
        return HeaderName

def RemoveHeader(HeadersToRemove, Print = True):

    if type(HeadersToRemove) is str:
        HeadersToRemove = [HeadersToRemove]
        
    for HeaderName in HeadersToRemove:
    
        HeaderName = CompleteHeaderName(HeaderName)

        Index = -1
        Found = False

        for File in Headers:
            Index += 1
            if File == HeaderName:
                
                del Headers[Index]
                
                if Print:
                    print("File \"" + HeaderName + "\" was excluded")
                    
                Found = True
                break
            
        if Print and not Found:   
            print("File \"" + HeaderName + "\" was not found")
            

def PrintHeaders():
    print()
    for File in Headers:
        print("\t"+File)
        
    print()

script_dir = os.path.dirname(__file__) #<-- absolute dir the script is in
rel_path = input("RelativeDirectory: ")
abs_file_path = os.path.join(script_dir, rel_path)

print(abs_file_path, "\n")

files = [f for f in listdir(abs_file_path) if isfile(join(abs_file_path, f))]
#print(files)



for File in files:
    NameStruct = File.split(".")
    if len(NameStruct) != 2:
        continue
    if NameStruct[1] != "h":
        continue
    Headers.append(File)

print("All Headers found in given directory:")

PrintHeaders()

print("You now can exclude certain Headers.\nType \"/def\" to exclude all Visual Studio default files")

while True:
    print()
    FileName = input("File to Exlude: ");
    print()
    if FileName == "/def":
        RemoveHeader(["resource", "resource1", "stdafx", "targetver", "pch"])
        
    elif FileName == "":
        break
    else:
        RemoveHeader(FileName)
        
   # print()
    PrintHeaders()

    
PrintHeaders()

for FileName in Headers:
    
    print("\n", FileName)
    
    try:
        File = open(FileName, "r")
    except IOError:
        print("Could not open File \"" + FileName + "\". Please check if it is still opened.")
        continue
    
    HeaderGuard = FileName[:-2].upper() + "_H"
    IsGuarded = False
    LineFound = False
    GuardLine = -1
    EndLines = 0
    
    
    OriginalData = [line for line in File]
    Data = [line.strip() for line in OriginalData]
    DataLen = len(Data)
    
    for Line in Data:
        if not LineFound:
            GuardLine += 1

        if Line == "":
            EndLines += 1
        else:
            EndLines = 0
            if HeaderGuard in Line:
               print("The Header \"" + FileName + "\" is already guarded.")
               IsGuarded = True
               break
            elif "#pragma" not in Line and Line != "":
                LineFound = True

            
    
    
    if IsGuarded:
        continue
    
    GuardLine -= 1
    if EndLines > 1:
        for Index in range(0, EndLines-1):
            del OriginalData[DataLen-1]
            DataLen -= 1
        OriginalData.append("#endif")
    else:
        OriginalData.append("\n\n#endif")
    
    
    if OriginalData[GuardLine-1] != "":
        OriginalData.insert(GuardLine, "\n")
        GuardLine += 1
        
    OriginalData.insert(GuardLine, "#ifndef " + HeaderGuard + "\n")
    OriginalData.insert(GuardLine+1, "#define " + HeaderGuard + "\n")

    File.close()

    try:
        File = open(FileName, "w")
    except IOError:
        print("Could not open File \"" + FileName + "\". Please check if it is still opened.")
        continue

    for Line in OriginalData:
        File.write(Line)

    File.close()

    
while True:
    pass



    
