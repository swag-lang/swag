#import "core"
using core.file

//#[swag.printbc]
#test
{
    loc := #location
    res := readAllLines(loc.fileName)
    @assert(res.err == null)
    @assert(res.lines.count > 5)
    @assert(res.lines[0] == @"#import "core""@)

    /*visit l: res.lines
    {
        console.print("LINE: %\n", l)
    }*/
}