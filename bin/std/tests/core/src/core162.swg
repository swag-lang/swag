#global skip
using Core.File

//#[Swag.PrintBc]
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
        @assert(res[0] == "u"'u8)
        @assert(res[1] == "s"'u8)
    }

    {
        catch reader.open(loc.fileName)
        @assert(@err == null)
        defer reader.close()

        var res: String
        @assert(reader.readLine(&res))
        @assert(res == @"using Core.File"@)

        while reader.readLine(&res)
        {
        }
    }
}