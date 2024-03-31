using Core, Core.Directory

//#[Swag.PrintBc]
#test
{
    var options: EnumerationOptions
    options.recurse = true

    let loc = #location.fileName
    let pth = Path.getDirectoryName(loc)

    let res   = assume enumerate(pth, options)
    var count = 0
    visit p: res
    {
        count += 1
        //Console.print("%\n", p.fullname)
    }

    @assert(count > 100)
}
