
using Core

//#[Swag.PrintBc]
#test
{
    let p = #location.fileName
    //Console.print("%\n", p)

    {
        var full = catch Path.getAbsolute(p)
        @assert(!@err())
        //Console.print("%\n", full)
    }

    {
        var cur = catch Directory.getCurrent()
        @assert(!@err())

        var full = catch Path.getAbsolute("..")
        @assert(!@err())

        //Console.print("%\n%\n", Path.getDirectoryName(cur), @countof(full))
        @assert(Path.getDirectoryName(cur) == full)
    }
}