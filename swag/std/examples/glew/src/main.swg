#import "core"
using core

const fctExtension = "glCompileShader "
var err:    ErrorId
var lines:  Array'String

func findLine(what: string)->*String
{
    visit *line: lines
    {
        (idx, success) := line.indexOf(what)
        if !success continue
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

    var line1: *String
    visit *line: lines
    {
        (idx, success) := line.indexOf(fctExtension)
        if !success continue
        console.print("line: %\n", :line)
        line1 = line
        break
    }

    if line1 == null
        error.fatal("error: cannot find '%'\n", fctExtension)

    (idx, success) := line1.indexOf("(")
    debug.assert(success)
    idx += 1

    var glew: String
    while line1.buffer[idx] != ")"'u8
    {
        glew += line1.buffer[idx]
        idx += 1
    }

    console.print("glew: %\n", glew)
    glew += ";"'u8

    var line2: *String
    visit *line: lines
    {
        (idx, success) = line.indexOf(glew.toString())
        if !success continue
        console.print("line: %\n", :line)
        line2 = line
        break
    }

    (idx, success) = line2.indexOf(" "'char)
    idx += 1

    var proc: String
    while line2.buffer[idx] != " "'u8
    {
        proc += line2.buffer[idx]
        idx += 1
    }

    console.print("proc: %\n", proc)

    var line3: *String
    visit *line: lines
    {
        (idx, success) = line.indexOf(proc.toString())
        if !success continue
        console.print("line: %\n", :line)
        line2 = line
        break
    }
}