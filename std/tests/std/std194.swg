#import "std"
using std.utf8

#test
{
    {
        res := compare("как поживаешь", "КАК ПОЖИВАЕШЬ", ComparisonType.UnicodeNoCase)
        @assert(res == 0)
    }

    {
        res := compare("как поживаешь", "КАК ПОЖИВАЕШЬ", ComparisonType.Unicode)
        @assert(res == 1)
    }

    {
        res := compare("КАК ПОЖИВАЕШЬ", "как поживаешь", ComparisonType.Unicode)
        @assert(res == -1)
    }
}