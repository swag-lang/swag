#import "core"
using core

//#[swag.printbc]
#test
{
    {
        stream := file.openRead("tolklkjlkjk:jto") catch {}
        @assert(@err != null)
    }

    {
        directory.setCurrent("fkdfjfkjfkdj") catch {}
        @assert(@err != null)
    }
}
