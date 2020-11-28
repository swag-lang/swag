#import "std"
using std.utf8

#test
{
    {
        res := compare("как поживаешь", "КАК ПОЖИВАЕШЬ", ComparisonType.UnicodeNoCase)
        @assert(res == 0)
        //console.print("%", res)
    }

    {
        res := compare("как поживаешь", "КАК ПОЖИВАЕШЬ", ComparisonType.Unicode)
        @assert(res > 0)
        //console.print("%", res)
    }
}