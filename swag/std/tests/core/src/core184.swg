#import "core"
using core

//#[swag.printbc]
#test
{
    {
        stream := catch file.openRead("tolklkjlkjk:jto")
        @assert(@err != null)
    }

    {
        catch directory.setCurrent("fkdfjfkjfkdj")
        @assert(@err != null)
    }
}
