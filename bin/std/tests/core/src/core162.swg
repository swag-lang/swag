using Core.File
// DO NOT CHANGE THE COMMENT
// DO NOT CHANGE THE COMMENT AGAIN

//#[Swag.PrintBc]
#test
{
    var reader: TextReader
    var loc = #location

    {
        catch reader.open(loc.fileName)
        @assert(@errmsg == null)
        defer reader.close()

        var res = reader.readToEnd()
        @assert(res.length > 10)
        @assert(res[0] == `u`'u8)
        @assert(res[1] == `s`'u8)
    }

    {
        catch reader.open(loc.fileName)
        @assert(@errmsg == null)
        defer reader.close()

        var res: String
        @assert(reader.readLine(&res))
        @assert(res == $"using Core.File"$)
        @assert(reader.readLine(&res))
        @assert(res == "// DO NOT CHANGE THE COMMENT")
        @assert(reader.readLine(&res))
        @assert(res == "// DO NOT CHANGE THE COMMENT AGAIN")

        while reader.readLine(&res)
        {
        }
    }
}