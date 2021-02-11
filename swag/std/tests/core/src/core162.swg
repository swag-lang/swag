#import "core"
using core.file

//#[swag.printbc]
#test
{
    var reader: TextReader
    loc := #location

    {
        catch reader.open(loc.fileName)
        @assert(@err == null)
        defer reader.close()

        res := reader.readToEnd()
        @assert(res.length > 10)
        @assert(res[0] == "#"'u8)
        @assert(res[1] == "i"'u8)
    }

    {
        catch reader.open(loc.fileName)
        @assert(@err == null)
        defer reader.close()

        res := reader.readLine()
        @assert(res == @"#import "core""@)

        while !res.isNull()
        {
            //console.print("LINE: %\n", res)
            //break
            res = reader.readLine()
        }
    }
}