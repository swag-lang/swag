#import "core"
using core

#main
{
    curPath := path.getDirectoryName(#location.fileName)
    glewPath := path.combine(curPath, "glew.h")
    (lines, err) := file.readAllLines(glewPath.toString())
    if err error.fatal("error: cannot read '%' (%)\n", glewPath, err)

    const fctExtension = "glCompileShader "

    visit *line: lines
    {
        (idx, success) := line.indexOf(fctExtension)
        if !success continue
        console.print("line: %\n", :line)
        break
    }
}