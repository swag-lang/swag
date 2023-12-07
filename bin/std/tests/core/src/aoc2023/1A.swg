// https://adventofcode.com/ 
// Year 2023

using Core

#test
{
    var dataPath: String = Path.getDirectoryName(#location.fileName)
    dataPath = Path.combine(dataPath, "1.txt")

    let lines = assume File.readAllLines(dataPath)
    var total = 0

    visit l: lines
    {
        let first = Utf8.indexOfAny(l, "0123456789")
        let last  = Utf8.lastIndexOfAny(l, "0123456789")
        total += ((l[first] - `0`) * 10) + (l[last] - `0`)
    }

    //Console.print("total: ", total)
    @assert(total == 55108)
}

