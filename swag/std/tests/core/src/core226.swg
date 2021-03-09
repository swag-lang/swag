
using core

//#[swag.printbc]
#test
{
    p := #location.fileName
    //console.print("%\n", p)

    {
        full := catch path.getAbsolute(p)
        @assert(!@err)
        //console.print("%\n", full)
    }

    {
        cur := catch directory.getCurrent()
        @assert(!@err)

        full := catch path.getAbsolute("..")
        @assert(!@err)

        //console.print("%\n%\n", path.getDirectoryName(cur), @countof(full))
        @assert(path.getDirectoryName(cur) == full)
    }
}