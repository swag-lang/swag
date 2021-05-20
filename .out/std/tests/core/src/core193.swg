
using core

#test
{
    {
        res := String.from("как поживаешь")
        res.makeUpper(CharacterSet.Unicode)
        @assert(res == "КАК ПОЖИВАЕШЬ")
        //Console.print("%", res)
    }

    {
        res := String.from("КАК ПОЖИВАЕШЬ")
        res.makeLower(CharacterSet.Unicode)
        @assert(res == "как поживаешь")
        //Console.print("%", res)
    }

    {
        res := String.from("КАК ПОЖИВАЕШЬ")
        @assert(res.toLower(CharacterSet.Unicode) == "как поживаешь")
        //Console.print("%", res)
    }
}