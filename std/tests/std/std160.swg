#import "std"
using std

//#[swag.printbc]
#test
{
    @assert(path.getExtension("loc.fileName") == ".fileName")
    @assert(path.getFileName("/") == null)

    loc := #location
    @assert(file.exists(loc.fileName))
    @assert(path.getExtension(loc.fileName) == ".swg")
    @assert(path.hasExtension(loc.fileName))

    @assert(path.getFileName(loc.fileName) == "std160.swg")
    //console.print("%\n", path.getFileNameWithoutExtension(loc.fileName))
    @assert(path.getFileNameWithoutExtension(loc.fileName) == "std160")

    var ext: String = path.getExtension(loc.fileName)
    @assert(ext.compare(".SWG", utf8.ComparisonType.AsciiNoCase) == 0)

    res := file.readAllText(loc.fileName)
    @assert(res.text.length > 0)
    //console.print("%", res.item0)
}