using core, Utf8, Tokenize

var lines:      Array'String
var toProcess:  Array'String

const LOG = false
const SINGLE = false

func findFirstLine(what: string)
{
    idxFirst := 0'uint
    visit line: lines
    {
        idx := line.indexOf(what)
        if idx == Swag.UInt.Max continue
        #if LOG Console.print("line: %\n", line)
        toProcess.add(dref line)
        idxFirst = @index + 1
        break
    }

    if toProcess.isEmpty() Console.fatal("error: cannot find '%'\n", what)

    #if !SINGLE
    {
        while !lines[idxFirst].isEmpty()
        {
            //Console.print("line: %\n", lines[idxFirst])
            toProcess.add(dref lines[idxFirst])
            idxFirst += 1
        }
    }
}

func findLine(what: string)->*String
{
    visit *line: lines
    {
        idx := line.indexOf(what)
        if idx == Swag.UInt.Max continue
        #if LOG Console.print("line: %\n", line)
        return line
    }

    Console.fatal("error: cannot find '%'\n", what)
    return lines[0]
}

func swapParams(what: string)->String
{
    var result: retval
    if what == "void, " return result

    var idxNext: uint
    loop
    {
        idxNext = what.indexOf(",", idxNext)
        if idxNext == Swag.UInt.Max break
        if !result.isEmpty() result += ", "

        // Isolate parameter name
        idxName := idxNext - 1
        idxEnd := idxNext

        var arr: string
        if what[idxName] == "]"'u8
        {
            while what[idxName] != "["'u8 idxName -= 1
            arr = what[idxName..idxNext - 1]
            idxEnd = idxName
            idxName -= 1
            #if LOG Console.print("arr: %\n", arr)
        }

        while Latin1.isLetter(what[idxName - 1]) idxName -= 1
        name := what[idxName..idxEnd-1]
        #if LOG Console.print("name: %\n", name)

        // Isolate parameter type
        idxType := idxName - 1
        while idxType and what[idxType - 1] != ","'u8 idxType -= 1
        if idxType idxType += 1
        type := String.from(what[idxType..idxName - 1])
        type.trim()
        type.insert(0, arr)
        #if LOG Console.print("type: '%'\n", type)

        // Pointer
        if(type.lastRune() == "*"'rune)
        {
            idxType = type.length - 2
            while Latin1.isSpace(type[idxType]) idxType -= 1
            while Latin1.isLetter(type[idxType]) idxType -= 1
            type.insert(idxType + 1, "*")
            type.length -= 2
            type.trim()
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
        Console.fatal("error: invalid number of arguments\n")

    var fctExtension: String = @args()[1] //"glCompileShader "
    fctExtension += " "
    Console.print("arg: '%'\n", fctExtension)

    ///////////////////
    curPath := Path.getDirectoryName(#location.fileName)
    glewPath := Path.combine(curPath, "glew.h")
    lines = catch File.readAllLines(glewPath)
    if @err Console.fatal("error: cannot read '%' (%)\n", glewPath, @err)

    findFirstLine(fctExtension)
    visit line1: toProcess
    {
        idxN := line1.indexOf(" ", 9)
        funcName := line1[8..idxN - 1]

        ///////////////////
        idx := line1.indexOf("(") + 1
        glewStr := String.from(cast(string) line1.getTo(")"'u8, idx, includeDelimiter: false))
        #if LOG Console.print("glewStr: %\n", glewStr)
        glewStr += ";"'u8

        ///////////////////
        line2 := findLine(glewStr)
        idx = line2.indexOf(" ") + 1
        proc := cast(string) line2.getTo(" "'u8, idx, includeDelimiter: false)
        #if LOG Console.print("proc: %\n", proc)

        line3 := findLine(proc)
        ///////////////////
        idx = line3.indexOf(" ")
        idx += 1
        retType := String.from(cast(string) line3.getTo(" "'u8, idx, includeDelimiter: false))
        if retType == "void" retType.clear()
        #if LOG Console.print("retType: %\n", retType)

        idx = line3.lastIndexOf("(")
        idx += 1
        proto := String.from(cast(string) line3.getTo(")"'u8, idx, includeDelimiter: false))
        proto += ", "

        ///////////////////
        strProto := swapParams(proto)
        if !retType.isEmpty()
            Console.print("func %(%)->%;\n", funcName, strProto, retType)
        else
            Console.print("func %(%);\n", funcName, strProto)

        ///////////////////
    }
}