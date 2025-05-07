using Core

//#[Swag.PrintBc]
#test
{
    {
        var stream = catch File.openRead("tolklkjlkjk:jto")
        @assert(@err != null)
    }

    {
        catch Directory.setCurrent("fkdfjfkjfkdj")
        @assert(@err != null)
    }
}
