using Core, Core.Directory

//#[Swag.PrintBc]
#test
{
    var options: EnumerationOptions
    options.recurse = true

    let loc = #curlocation.fileName
    let pth = Path.getDirectoryName(loc)

    let res   = assume enumerate(pth, options)
    var count = 0
    foreach p in res
    {
        count += 1
        //Console.print("%\n", p.fullname)
    }

    @assert(count > 100)
}
