#import "std"
using std.file

//#[swag.printbc]
#test
{
    var reader: TextReader
    loc := #location

    {
        f := reader.open(loc.fileName)
        @assert(f == null)
        defer reader.close()

        res := reader.readToEnd()
        @assert(res.length > 10)
        @assert(res[0] == "#"'u8)
        @assert(res[1] == "i"'u8)
    }

    {
        @assert(reader.open(loc.fileName) == null)
        defer reader.close()

        res := reader.readLine()
        @assert(res == @"#import "std""@)

        while !res.isNull()
        {
            //console.print("LINE: %\n", res)
            //break
            res = reader.readLine()
        }
    }
}