#import "std"
using std

//#[swag.printbc]
#test
{
    loc := #location
    @assert(file.exists(loc.fileName))
    @assert(path.getExtension(loc.fileName) == ".swg")
    @assert(path.hasExtension(loc.fileName))

    var ext: String = path.getExtension(loc.fileName)
    @assert(ext.compare(".SWG", utf8.ComparisonType.AsciiIgnoreCase) == 0)

    res := file.readAllText(loc.fileName)
    //console.print("%", res)
}