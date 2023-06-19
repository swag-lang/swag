
using Core.File

//#[Swag.PrintBc]
#test
{
    loc := #location
    res := catch readAllLines(loc.fileName)
    @assert(@errmsg == null)
    @assert(res.count > 5)
    @assert(res[0] == @""@)

    /*visit l: res.lines
    {
        Console.print("LINE: %\n", l)
    }*/
}