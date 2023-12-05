// https://adventofcode.com/ 
// Year 2023

using Core

#test
{
    var dataPath: String = Path.getDirectoryName(#location.fileName)
    dataPath = Path.combine(dataPath, "4.txt")

    let lines = assume File.readAllLines(dataPath)

    var total = 0'u64

    Console.print("total: ", total)
    //@assert(total == 12263631)
}

