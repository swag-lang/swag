#import "std"
using std

//#[swag.printbc]
#test
{
    @assert(path.getExtension("loc.fileName") == ".fileName")

    loc := #location
    @assert(file.exists(loc.fileName))
    @assert(path.getExtension(loc.fileName) == ".swg")
    @assert(path.hasExtension(loc.fileName))

    //console.print("-%-\n", @countof(path.getFileName(loc.fileName)))

    @assert(path.getFileName(loc.fileName) == "std160.swg")

    var ext: String = path.getExtension(loc.fileName)
    @assert(ext.compare(".SWG", utf8.ComparisonType.AsciiNoCase) == 0)

    res := file.readAllText(loc.fileName)
    //console.print("%", res)
}