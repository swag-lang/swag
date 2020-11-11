#import "std"
using std

#test
{
    loc := #location
    dir := path.getDirectoryName(loc.fileName)

    res := directory.enumerateFiles(dir, "*.*")
}