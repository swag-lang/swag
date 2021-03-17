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
        @assert(res[0] == "u"'u8)
        @assert(res[1] == "s"'u8)
    }

    {
        catch reader.open(loc.fileName)
        @assert(@err == null)
        defer reader.close()

        res := reader.readLine()
        @assert(res == @"using core.file"@)

        while !res.isNull()
        {
            //console.print("LINE: %\n", res)
            //break
            res = reader.readLine()
        }
    }
}