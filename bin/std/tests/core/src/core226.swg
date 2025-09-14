using Core

//#[Swag.PrintBc]
#test
{
    let p = #curlocation.fileName
    //Console.print("%\n", p)

    {
        var full = catch Path.getAbsolute(p)
        @assert(@err == null)
        //Console.print("%\n", full)
    }

    {
        let cur = catch Directory.getCurrent()
        @assert(@err == null)

        let full = catch Path.getAbsolute("..")
        @assert(@err == null)

        //Console.print("%\n%\n", Path.getDirectoryName(cur), @countof(full))
        @assert(Path.getDirectoryName(cur) == full)
    }
}
