import os

# pip install pcpp
# https://pypi.org/project/pcpp
from pcpp import Preprocessor

shaderExt = [".d.hlsl", ".d.glsl"]


def needBuildShader(filename):
    for fileType in shaderExt:
        if filename.endswith(fileType):
            return True
    return False


def buildShader(dirName):
    outputFile = None
    inputFile = None
    for root, dirs, files in os.walk(dirName):
        for file in files:
            if needBuildShader(file):
                outputFile = file
                outputFile = outputFile.replace(".d.hlsl", ".hlsl")
                outputFile = outputFile.replace(".d.glsl", ".glsl")

                print("%s <- %s" % (outputFile, file))

                outputFile = root + "/" + outputFile
                inputFile = root + "/" + file

                p = Preprocessor()
                p.compress = 2
                p.line_directive = None

                with open(inputFile, 'rt') as finput:
                    p.parse(finput.read(), inputFile)
                with open(outputFile, 'w') as foutput:
                    p.write(foutput)


def main():
    directory = "."
    for filename in os.listdir(directory):
        if os.path.isdir(filename):
            buildShader(filename)
            continue
        else:
            continue


if __name__ == '__main__':
    main()
