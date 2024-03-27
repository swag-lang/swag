using Core

#test
{
    {
        var res = String.from("как поживаешь")
        res.makeUpper(CharacterSet.Unicode)
        @assert(res == "КАК ПОЖИВАЕШЬ")
    }

    {
        var res = String.from("КАК ПОЖИВАЕШЬ")
        res.makeLower(CharacterSet.Unicode)
        @assert(res == "как поживаешь")
    }

    {
        var res = String.from("КАК ПОЖИВАЕШЬ")
        @assert(res.toLower(CharacterSet.Unicode) == "как поживаешь")
    }
}
