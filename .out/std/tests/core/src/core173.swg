
using core

#test
{
    loc := #location
    dir := Path.getDirectoryName(loc.fileName)
    res := catch Directory.enumerateDirectories(@"F:\swag\std\output"@, recurse: true)
    //@assert(res.count == 1)
    //visit *c: res
    //    Console.print("FF: % %\n", c.fullname, c.size)
}