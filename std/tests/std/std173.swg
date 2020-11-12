#import "std"
using std

#test
{
    loc := #location
    dir := path.getDirectoryName(loc.fileName)
    res := directory.enumerateFiles(dir)
    //@assert(res.count == 1)
    visit *c: res
        console.print("FF: % %\n", c.fullname, c.size)
}