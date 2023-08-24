
using Core.File

//#[Swag.PrintBc]
#test
{
    var loc = #location
    var res = catch readAllLines(loc.fileName)
    @assert(@errmsg == null)
    @assert(res.count > 5)
    @assert(res[0] == $""$)

    /*visit l: res.lines
    {
        Console.print("LINE: %\n", l)
    }*/
}