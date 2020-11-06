#import "std"
using std

//#[swag.printbc]
#test
{
    loc := #location
    info := file.getInfo(loc.fileName)
    console.print("size: %\n", info.size)
    console.print("attr: %\n", info.attributes)
    console.print("crt:  %\n", info.creationTime)
    console.print("lwt:  %\n", info.lastWriteTime)
}