
using core.file

//#[Swag.printbc]
#test
{
    loc := #location
    res := path.getDirectoryName(loc.fileName)
    @assert(@countof(res) != 0)
    //Console.print("%\n", res)
    res1 := path.getRootName(loc.fileName)
    //Console.print("%\n", res1)
    @assert(@countof(res1) != 0)

    @assert(path.isRooted("/"))
    @assert(path.isRooted("c:/"))
    @assert(path.isRooted(loc.fileName))

    var t: String = loc.fileName
    @assert(path.hasExtension(t.toString()))
}