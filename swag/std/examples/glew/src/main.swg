#import "core"
using core

#main
{
    curPath := path.getDirectoryName(#location.fileName)
    glewPath := path.combine(curPath, "glew.h")
    (lines, err) := file.readAllLines(glewPath.toString())
    if err
    {
        console.print("error: cannot read '%' (%)\n", glewPath, err)
        environment.exit(cast(u32) -1)
    }

    console.print("lines: %\n", lines.count)
}