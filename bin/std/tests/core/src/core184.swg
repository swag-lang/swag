
using Core

//#[Swag.PrintBc]
#test
{
    {
        stream := catch File.openRead("tolklkjlkjk:jto")
        @assert(@errmsg != null)
    }

    {
        catch Directory.setCurrent("fkdfjfkjfkdj")
        @assert(@errmsg != null)
    }
}
