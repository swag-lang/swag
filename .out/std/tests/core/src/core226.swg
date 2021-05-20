
using core

//#[Swag.printbc]
#test
{
    p := #location.fileName
    //Console.print("%\n", p)

    {
        full := catch Path.getAbsolute(p)
        @assert(!@err)
        //Console.print("%\n", full)
    }

    {
        cur := catch Directory.getCurrent()
        @assert(!@err)

        full := catch Path.getAbsolute("..")
        @assert(!@err)

        //Console.print("%\n%\n", Path.getDirectoryName(cur), @countof(full))
        @assert(Path.getDirectoryName(cur) == full)
    }
}