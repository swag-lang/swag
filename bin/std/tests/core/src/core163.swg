//toto
using Core.File

//#[Swag.PrintBc]
#test
{
    var loc = #location
    var res = catch readAllLines(loc.fileName)
    @assert(@err() == null)
    @assert(res.count > 5)
    @assert(res[0] == "//toto")

    /*visit l: res.lines
    {
        Console.print("LINE: %\n", l)
    }*/
}
