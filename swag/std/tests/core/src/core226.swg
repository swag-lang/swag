#import "core"
using core

//#[swag.printbc]
#test
{
    p := #location.fileName
    //console.print("%\n", p)

    {
        full := path.getAbsolute(p)
        @assert(!@err)
        //console.print("%\n", full)
    }

    {
        cur := directory.getCurrent()
        @assert(!@err)

        full := path.getAbsolute("..")
        @assert(!@err)

        //console.print("%\n%\n", path.getDirectoryName(cur), @countof(full))
        @assert(path.getDirectoryName(cur) == full)
    }
}