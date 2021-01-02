#import "core"
using core, utf8

var err:        ErrorId
var lines:      Array'String
var version:    String

func findLine(what: string, checkVersion = false)->const &String
{
    visit line: lines
    {
        idx := line.indexOf(what)
        if idx == swag.MaxUINT continue
        console.print("line: %\n", line)

        if checkVersion
        {
            idx1 := @index
            while idx1
            {
                line1 := lines[idx1]
                idxO := line1.indexOf("GL_VERSION")
                if idxO != swag.MaxUINT
                {
                    version = String.from(cast(string) utf8.eatTo(line1, " "'u8, idxO))
                    break
                }

                idx1 -= 1
            }
        }

        return line
    }

    error.fatal("error: cannot find '%'\n", what)
    return lines[0]
}

#main
{
    ///////////////////
    if @countof(@args()) <= 1
        error.fatal("error: invalid number of arguments\n")

    var fctExtension: String = @args()[1] //"glCompileShader "
    fctExtension += " "
    console.print("arg: '%'\n", fctExtension)

    ///////////////////
    curPath := path.getDirectoryName(#location.fileName)
    glewPath := path.combine(curPath, "glew.h")
    (lines, err) = file.readAllLines(glewPath)
    if err error.fatal("error: cannot read '%' (%)\n", glewPath, err)

    ///////////////////
    line1 := findLine(fctExtension, true)
    idx := line1.indexOf("(") + 1
    glew := String.from(cast(string) line1.eatTo(")"'u8, idx, includeDelimiter: false))
    console.print("glew: %\n", glew)
    glew += ";"'u8

    ///////////////////
    line2 := findLine(glew)
    idx = line2.indexOf(" ") + 1
    proc := cast(string) line2.eatTo(" "'u8, idx, includeDelimiter: false)
    console.print("proc: %\n", proc)

    ///////////////////
    line3 := findLine(proc)
    idx = line3.lastIndexOf("(")
    proto := cast(string) line3.eatTo(")"'u8, idx, includeDelimiter: true)

    ///////////////////
    console.print("\n")
    console.print("version: %\n", version)
    console.print("proto: % %\n", @args()[1], proto)

    ///////////////////
}