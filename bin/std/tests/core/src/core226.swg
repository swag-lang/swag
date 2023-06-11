
using Core

//#[Swag.PrintBc]
#test
{
    p := #location.fileName
    //Console.print("%\n", p)

    {
        full := catch Path.getAbsolute(p)
        @assert(!@errmsg)
        //Console.print("%\n", full)
    }

    {
        cur := catch Directory.getCurrent()
        @assert(!@errmsg)

        full := catch Path.getAbsolute("..")
        @assert(!@errmsg)

        //Console.print("%\n%\n", Path.getDirectoryName(cur), @countof(full))
        @assert(Path.getDirectoryName(cur) == full)
    }
}