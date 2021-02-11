#import "core"
using core.file

//#[swag.printbc]
#test
{
    loc := #location
    res := readAllLines(loc.fileName) catch {}
    @assert(@err == null)
    @assert(res.count > 5)
    @assert(res[0] == @"#import "core""@)

    /*visit l: res.lines
    {
        console.print("LINE: %\n", l)
    }*/
}