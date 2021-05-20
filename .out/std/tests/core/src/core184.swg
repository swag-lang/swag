
using core

//#[Swag.printbc]
#test
{
    {
        stream := catch file.openRead("tolklkjlkjk:jto")
        @assert(@err != null)
    }

    {
        catch Directory.setCurrent("fkdfjfkjfkdj")
        @assert(@err != null)
    }
}
