using Core, Utf8, Tokenize

var lines:      Array'String
var toProcess:  Array'String

const LOG = false
const SINGLE = false

func findFirstLine(what: string)
{
    var idxFirst = 0'u64
    visit line: lines
    {
        let idx = line.indexOf(what)
        if idx == Swag.U64.Max continue
        #if LOG Console.printf("line: %\n", line)
        toProcess.add(line)
        idxFirst = @index + 1
        break
    }

    if toProcess.isEmpty()
        Console.fatal("error: cannot find '%'\n", what)

    #if !SINGLE
    {
        while !lines[idxFirst].isEmpty()
        {
            //Console.printf("line: %\n", lines[idxFirst])
            toProcess.add(lines[idxFirst])
            idxFirst += 1
        }
    }
}

func findLine(what: string)->*String
{
    visit &line: lines
    {
        let idx = line.indexOf(what)
        if idx == Swag.U64.Max continue
        #if LOG Console.printf("line: %\n", line)
        return line
    }

    Console.fatal("error: cannot find '%'\n", what)
    return lines[0]
}

func swapParams(what: string)->String
{
    var result: retval
    if what == "void, " return result

    var idxNext: u64
    loop
    {
        idxNext = what.indexOf(",", idxNext)
        if idxNext == Swag.U64.Max break
        if !result.isEmpty() result += ", "

        // Isolate parameter name
        var idxName = idxNext - 1
        var idxEnd = idxNext

        var arr: string
        if what[idxName] == `]`
        {
            while what[idxName] != `[` idxName -= 1
            arr = what[idxName..idxNext - 1]
            idxEnd = idxName
            idxName -= 1
            #if LOG Console.printf("arr: %\n", arr)
        }

        while Latin1.isLetter(what[idxName - 1]) idxName -= 1
        let name = what[idxName..idxEnd-1]
        #if LOG Console.printf("name: %\n", name)

        // Isolate parameter type
        var idxType = idxName - 1
        while idxType and what[idxType - 1] != `,` idxType -= 1
        if idxType idxType += 1
        var type = String.from(what[idxType..idxName - 1])
        type.trim()
        type.insert(0, arr)
        #if LOG Console.printf("type: '%'\n", type)

        // Pointer
        if type.lastRune() == `*`'rune
        {
            var idxType1 = cast(s32) (type.length - 2)
            while idxType1 >= 0 and Latin1.isSpace(type[idxType1])
                idxType1 -= 1
            while idxType1 >= 0 and Latin1.isLetter(type[idxType1])
                idxType1 -= 1

            type.insert(cast(u64) (idxType1 + 1), "*")
            type.length -= 1
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
    {
        Console.fatal("error: invalid number of arguments\n")
        Console.fatal("you must specify a gl function (ex: glCompileShader)\n")
    }

    var fctExtension: String = @args()[1] //"glCompileShader"
    fctExtension.trim()
    fctExtension += " "
    Console.printf("arg: '%'\n", fctExtension)

    ///////////////////
    let curPath = Path.getDirectoryName(#location.fileName)
    var glewPath = Path.combine(curPath, "glew.h")
    lines = catch File.readAllLines(glewPath)
    if @errmsg Console.fatal("error: cannot read '%' (%)\n", glewPath, @errmsg)

    findFirstLine(fctExtension)
    visit line11: toProcess
    {
        let line1 = cast(string) line11
        let idxN = line1.indexOf(" ", 9)
        let funcName = line1[8..idxN - 1]

        ///////////////////
        var idx = line1.indexOf("(") + 1
        var glewStr = String.from(cast(string) line1.getTo(`)`, idx, includeDelimiter: false))
        #if LOG Console.printf("glewStr: %\n", glewStr)
        glewStr += `;`'u8

        ///////////////////
        let line2 = findLine(glewStr)
        idx = line2.indexOf(" ") + 1
        let proc = cast(string) line2.getTo(` `, idx, includeDelimiter: false)
        #if LOG Console.printf("proc: %\n", proc)

        let line3 = findLine(proc)
        ///////////////////
        idx = line3.indexOf(" ")
        idx += 1
        var retType = String.from(cast(string) line3.getTo(` `, idx, includeDelimiter: false))
        if retType == "void" retType.clear()
        #if LOG Console.printf("retType: %\n", retType)

        idx = line3.lastIndexOf("(")
        idx += 1
        var proto = String.from(cast(string) line3.getTo(`)`, idx, includeDelimiter: false))
        proto += ", "

        ///////////////////
        var strProto = swapParams(proto)
        if !retType.isEmpty()
            Console.printf("func %(%)->%;\n", funcName, strProto, retType)
        else
            Console.printf("func %(%);\n", funcName, strProto)

        ///////////////////
    }
}