using Core.File

//#[Swag.PrintBc]
#test
{
    var loc = #curlocation
    let res = Path.getDirectoryName(loc.fileName)
    @assert(@countof(res) != 0)
    //Console.print("%\n", res)
    let res1 = Path.getRootName(loc.fileName)
    //Console.print("%\n", res1)
    @assert(@countof(res1) != 0)

    @assert(Path.isRooted("/"))
    @assert(Path.isRooted("c:/"))
    @assert(Path.isRooted(loc.fileName))

    var t: String = loc.fileName
    @assert(Path.hasExtension(t.toString()))
}
