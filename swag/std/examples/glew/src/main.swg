#import "core"
using core

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
    (lines, err) = file.readAllLines(glewPath.toString())
    if err error.fatal("error: cannot read '%' (%)\n", glewPath, err)

    line1 := findLine(fctExtension)
    debug.assert(line1 != null)

    idx := line1.indexOf("(")
    debug.assert(idx != swag.MaxUINT)
    idx += 1

    var glew: String
    while line1.buffer[idx] != ")"'u8
    {
        glew += line1.buffer[idx]
        idx += 1
    }

    console.print("glew: %\n", glew)
    glew += ";"'u8

    line2 := findLine(glew.toString())
    debug.assert(line2 != null)

    idx = line2.indexOf(" ")
    idx += 1

    var proc: String
    while line2.buffer[idx] != " "'u8
    {
        proc += line2.buffer[idx]
        idx += 1
    }

    console.print("proc: %\n", proc)

    line3 := findLine(proc.toString())
    debug.assert(line3 != null)
}