#import "core"
using core

#test
{
    loc := #location
    dir := path.getDirectoryName(loc.fileName)
    res := directory.enumerateDirectories(@"F:\swag\std\output"@, recurse: true)
    //@assert(res.count == 1)
    //visit *c: res
    //    console.print("FF: % %\n", c.fullname, c.size)
}