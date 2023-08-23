
using Core

#test
{
    var loc = #location
    var dir = Path.getDirectoryName(loc.fileName)
    var res = catch Directory.enumerateDirectories(@"F:\swag\std\output"@, recurse: true)
    //@assert(res.count == 1)
    //visit &c: res
    //    Console.print("FF: % %\n", c.fullname, c.size)
}