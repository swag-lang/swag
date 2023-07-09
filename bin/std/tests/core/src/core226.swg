
using Core

//#[Swag.PrintBc]
#test
{
    var p = #location.fileName
    //Console.print("%\n", p)

    {
        var full = catch Path.getAbsolute(p)
        @assert(!@errmsg)
        //Console.print("%\n", full)
    }

    {
        var cur = catch Directory.getCurrent()
        @assert(!@errmsg)

        var full = catch Path.getAbsolute("..")
        @assert(!@errmsg)

        //Console.print("%\n%\n", Path.getDirectoryName(cur), @countof(full))
        @assert(Path.getDirectoryName(cur) == full)
    }
}