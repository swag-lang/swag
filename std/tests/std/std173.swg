#import "std"
using std

#test
{
    loc := #location
    dir := path.getDirectoryName(loc.fileName)
    res := directory.enumerateFiles(dir, "*.*")
    visit *c: res
        console.print("%\n", c.fullname)
}