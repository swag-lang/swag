//toto
using Core.File

//#[Swag.PrintBc]
#test
{
    var loc = #curlocation
    var res = catch readAllLines(loc.fileName)
    @assert(@err() == null)
    @assert(res.count > 5)
    @assert(res[0] == "//toto")

    /*foreach l in res.lines
    {
        Console.print("LINE: %\n", l)
    }*/
}
