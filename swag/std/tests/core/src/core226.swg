#import "core"
using core

//#[swag.printbc]
#test
{
    p := #location.fileName
    //console.print("%\n", p)

    {
        full := path.getAbsolute(p) catch {}
        @assert(!@err)
        //console.print("%\n", full)
    }

    {
        cur := directory.getCurrent() catch {}
        @assert(!@err)

        full := path.getAbsolute("..") catch {}
        @assert(!@err)

        //console.print("%\n%\n", path.getDirectoryName(cur), @countof(full))
        @assert(path.getDirectoryName(cur) == full)
    }
}