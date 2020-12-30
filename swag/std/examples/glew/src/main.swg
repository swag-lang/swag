#import "core"
using core

#main
{
    curPath := path.getDirectoryName(#location.fileName)
    glewPath := path.combine(curPath, "glew.h")
    (lines, err) := file.readAllLines(glewPath.toString())
    if err error.fatal("error: cannot read '%' (%)\n", glewPath, err)

    const fctExtension = "glCompileShader "

    var line1: *String
    visit *line: lines
    {
        (idx, success) := line.indexOf(fctExtension)
        if !success continue
        console.print("line: %\n", :line)
        line1 = line
        break
    }

    (idx, success) := line1.indexOf("(")
    debug.assert(success)
    idx += 1

}