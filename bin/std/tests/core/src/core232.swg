
using Core, Directory

//#[Swag.PrintBc]
#test
{
    var options: EnumerationOptions
    options.recurse = true

    var loc = #location.fileName
    var pth = Path.getDirectoryName(loc)

    var res = assume enumerate(pth, options)
    var count = 0
    visit p: res
    {
        count += 1
        //Console.print("%\n", p.fullname)
    }

    @assert(count > 100)
}