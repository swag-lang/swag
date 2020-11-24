#import "std"
using std.file

//#[swag.printbc]
#test
{
    loc := #location
    res := readAllLines(loc.fileName)
    @assert(res.err == null)
    @assert(res.lines.count > 5)

    visit l: res.lines
    {
        //console.print("LINE: %\n", l)
    }
}