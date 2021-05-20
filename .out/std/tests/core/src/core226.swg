
using core

//#[Swag.printbc]
#test
{
    p := #location.fileName
    //Console.print("%\n", p)

    {
        full := catch path.getAbsolute(p)
        @assert(!@err)
        //Console.print("%\n", full)
    }

    {
        cur := catch Directory.getCurrent()
        @assert(!@err)

        full := catch path.getAbsolute("..")
        @assert(!@err)

        //Console.print("%\n%\n", path.getDirectoryName(cur), @countof(full))
        @assert(path.getDirectoryName(cur) == full)
    }
}