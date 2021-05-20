
using core.file

//#[Swag.printbc]
#test
{
    loc := #location
    res := catch readAllLines(loc.fileName)
    @assert(@err == null)
    @assert(res.count > 5)
    @assert(dref res[0] == @""@)

    /*visit l: res.lines
    {
        Console.print("LINE: %\n", l)
    }*/
}