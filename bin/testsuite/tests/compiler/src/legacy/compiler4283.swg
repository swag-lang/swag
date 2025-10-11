#test
{
    #[Swag.ConstExpr]
    func getType(str: bool)->typeinfo
    {
        return str ? string : s32
    }

    var x: #decltype(getType(false))
    #assert(#typeof(x) == s32)
    x = 0

    var x1: #decltype(getType(true))
    #assert(#typeof(x1) == string)
    x1 = "0"
}
