/*
--- Part Two ---
Now, take the signal you got on wire a, override wire b to that signal, and reset the other wires (including wire a). What new signal is ultimately provided to wire a?
*/

using Core

#ast
{
    let dataPath = Path.combine(Path.getDirectoryName(#file), "7.txt")

    var result: String
    var r:      Parser.RegExp

    var map: HashTable'(string, string)
    map = [{"AND", "&"}, {"OR", "|"}, {"LSHIFT", "<<"}, {"RSHIFT", ">>"}]

    assume
    {
        foreach l in File.readAllLines(dataPath)
        {
            l.makeUpper()
            result += "private const "
            if r.match(#"(\w+) -> B"#, l) do
                result += Format.toString("B = %", 46065)
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
#assert(A == 14134)
