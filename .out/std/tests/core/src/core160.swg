
using core

//#[Swag.printbc]
#test
{
    @assert(path.getExtension("loc.fileName") == ".fileName")
    @assert(path.getFileName("/") == null)

    loc := #location
    @assert(File.exists(loc.fileName))
    @assert(path.getExtension(loc.fileName) == ".swg")
    @assert(path.hasExtension(loc.fileName))

    @assert(path.getFileName(loc.fileName) == "core160.swg")
    //Console.print("%\n", path.getFileNameWithoutExtension(loc.fileName))
    @assert(path.getFileNameWithoutExtension(loc.fileName) == "core160")

    var ext: String = path.getExtension(loc.fileName)
    @assert(utf8.compare(ext.toString(), ".SWG", utf8.ComparisonType.Latin1NoCase) == 0)

    res := catch File.readAllText(loc.fileName)
    @assert(res.length > 0)
    //Console.print("%", res.item0)
}