#import "core"
using core

//#[swag.printbc]
#test
{
    loc := #location
    info := file.getInfo(loc.fileName) catch {}
    /*console.print("size: %\n", info.size)
    console.print("attr: %\n", info.attributes)
    console.print("crt:  %\n", info.creationTime)
    console.print("lwt:  %\n", info.lastWriteTime)*/
}