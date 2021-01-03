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

func swapParams(what: string)->String
{
    var result: retval

    var idxNext: uint
    while true
    {
        idxNext = what.indexOf(",", idxNext)
        if idxNext == swag.MaxUINT break
        if !result.isEmpty() result += ", "

        // Isolate parameter name
        idxName := idxNext - 1
        while latin1.isLetter(what[idxName - 1]) idxName -= 1
        name := what[idxName..idxNext - 1]
        console.print("name: %\n", name)

        // Isolate parameter type
        idxType := idxName - 1
        while idxType && what[idxType - 1] != ","'u8 idxType -= 1
        if idxType idxType += 1
        type := String.from(what[idxType..idxName - 1])
        type.trim()
        console.print("type: %\n", type)

        // Pointer
        if(type.lastChar() == "*"'char)
        {
            idxType = type.length - 1
            while latin1.isLetter(type[idxType]) idxType -= 1
            type.length -= 2
            type.insert("*", idxType)
        }

        result += name
        result += ": "
        result += type

        idxNext += 1
    }

    return result
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
    idx += 1
    proto := String.from(cast(string) line3.eatTo(")"'u8, idx, includeDelimiter: false))
    proto += ", "

    ///////////////////
    strProto := swapParams(proto)
    console.print("\n")
    console.print("version: %\n", version)
    console.print("func %(%);\n", @args()[1], strProto)

    ///////////////////
}