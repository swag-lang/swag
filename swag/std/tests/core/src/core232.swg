#import "core"
using core, directory

#test
{
    var options: EnumerationOptions
    options.recurse = true

    loc := #location.fileName
    pth := path.getDirectoryName(loc)

    res := enumerate(pth, options)
    count := 0
    visit p: res
    {
        count += 1
        //console.print("%\n", p.fullname)
    }

    @assert(count > 100)
}