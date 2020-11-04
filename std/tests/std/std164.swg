#import "std"
using std.file

//#[swag.printbc]
#test
{
    loc := #location
    res := path.getDirectoryName(loc.fileName)
    @assert(@countof(res) != 0)
    //console.print("%\n", res)
    res1 := path.getRootName(loc.fileName)
    //console.print("%\n", res1)
    @assert(@countof(res1) != 0)
}