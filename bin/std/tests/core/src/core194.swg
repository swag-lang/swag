using Core.Utf8

#test
{
    {
        let res = compare("как поживаешь", "КАК ПОЖИВАЕШЬ", ComparisonType.UnicodeNoCase)
        @assert(res == 0)
    }

    {
        let res = compare("как поживаешь", "КАК ПОЖИВАЕШЬ", ComparisonType.Unicode)
        @assert(res == 1)
    }

    {
        let res = compare("КАК ПОЖИВАЕШЬ", "как поживаешь", ComparisonType.Unicode)
        @assert(res == -1)
    }
}
