#import "std"
using std.file

//#[swag.printbc]
#test
{
    var reader: TextReader
    loc := #location

    {
        @assert(reader.open(loc.fileName, TextReader.Encoding.Utf8))
        defer reader.close()

        res := reader.readToEnd()
        //console.print("%\n", res)
    }

    {
        @assert(reader.open(loc.fileName, TextReader.Encoding.Utf8))
        defer reader.close()

        res := reader.readLine()
        while !res.isNull()
        {
            //console.print("LINE: %\n", res)
            res = reader.readLine()
        }
    }
}