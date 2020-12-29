#import "core"
using core

#main
{
    loc := #location
    curPath := loc.fileName
    curPath = path.getDirectoryName(curPath)
    glewPath := path.combine(curPath, "glew.h")
    res := file.readAllLines(glewPath.toString())
}