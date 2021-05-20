
using core, Directory

//#[Swag.printbc]
#test
{
    var options: EnumerationOptions
    options.recurse = true

    loc := #location.fileName
    pth := path.getDirectoryName(loc)

    res := assume enumerate(pth, options)
    count := 0
    visit p: res
    {
        count += 1
        //Console.print("%\n", p.fullname)
    }

    @assert(count > 100)
}