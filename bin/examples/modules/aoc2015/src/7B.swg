/*
--- Part Two ---
Now, take the signal you got on wire a, override wire b to that signal, and reset the other wires (including wire a). What new signal is ultimately provided to wire a?
*/

using Core

#ast
{
    let dataPath = Path.combine(Path.getDirectoryName(#file), "7.txt")

    var result: String
    var r: Parser.RegExp

    var map: HashTable'(string, string)
    map.add("AND", "&")
    map.add("OR", "|")
    map.add("LSHIFT", "<<")
    map.add("RSHIFT", ">>")

    assume
    {
        visit l: File.readAllLines(dataPath)
        {
            l.replace("is", "_is")
            l.replace("if", "_if")
            l.replace("do", "_do")
            l.replace("in", "_in")

            result += "private const "
            if r.match(#"(\w+) -> b"#, l) do
                result += Format.toString("b = %", 46065)
            elif r.match(#"(\w+) -> (\w+)"#, l) do
                result += Format.toString("% = %", r[2], r[1])
            elif r.match(#"(\w+) (\w+) (\w+) -> (\w+)"#, l) do
                result += Format.toString("% = % % %", r[4], r[1], map.find(r[2]).value, r[3])
            elif r.match(#"NOT (\w+) -> (\w+)"#, l) do
                result += Format.toString("% = ~%", r[2], r[1])
            result += "\n"
        }
    }

    return result
}

//#print a
#assert a == 14134