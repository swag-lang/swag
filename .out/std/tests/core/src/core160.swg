
using Core

//#[Swag.printbc]
#test
{
    @assert(Path.getExtension("loc.fileName") == ".fileName")
    @assert(Path.getFileName("/") == null)

    loc := #location
    @assert(File.exists(loc.fileName))
    @assert(Path.getExtension(loc.fileName) == ".swg")
    @assert(Path.hasExtension(loc.fileName))

    @assert(Path.getFileName(loc.fileName) == "core160.swg")
    //Console.print("%\n", Path.getFileNameWithoutExtension(loc.fileName))
    @assert(Path.getFileNameWithoutExtension(loc.fileName) == "core160")

    var ext: String = Path.getExtension(loc.fileName)
    @assert(Utf8.compare(ext.toString(), ".SWG", Utf8.ComparisonType.Latin1NoCase) == 0)

    res := catch File.readAllText(loc.fileName)
    @assert(res.length > 0)
    //Console.print("%", res.item0)
}