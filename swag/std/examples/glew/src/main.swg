#import "core"
using core

#main
{
    curPath := path.getDirectoryName(#location.fileName)
    glewPath := path.combine(curPath, "glew.h")
    (lines, err) := file.readAllLines(glewPath.toString())
    console.print("lines: %\n", lines.count)
}