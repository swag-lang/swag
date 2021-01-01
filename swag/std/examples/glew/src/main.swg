#import "core"
using core, utf8

const fctExtension = "glCompileShader "
var err:    ErrorId
var lines:  Array'String

func findLine(what: string)->*String
{
    visit *line: lines
    {
        idx := line.indexOf(what)
        if idx == swag.MaxUINT continue
        console.print("line: %\n", :line)
        return line
    }

    return null
}

#main
{
    curPath := path.getDirectoryName(#location.fileName)
    glewPath := path.combine(curPath, "glew.h")
    (lines, err) = file.readAllLines(glewPath)
    if err error.fatal("error: cannot read '%' (%)\n", glewPath, err)

    line1 := findLine(fctExtension)
    debug.assert(line1 != null)

    idx := line1.indexOf("(") + 1
    glew := String.from(line1.eatTo(")"'u8, idx, includeDelimiter: false))
    console.print("glew: %\n", glew)
    glew += ";"'u8

    line2 := findLine(glew)
    debug.assert(line2 != null)

    idx = line2.indexOf(" ") + 1
    proc := cast(string) line2.eatTo(" "'u8, idx, includeDelimiter: false)
    console.print("proc: %\n", proc)

    line3 := findLine(proc)
    debug.assert(line3 != null)

    idx = line3.lastIndexOf("(")
    proto := cast(string) line3.eatTo(")"'u8, idx, includeDelimiter: true)
    console.print("proto: %\n", proto)
}