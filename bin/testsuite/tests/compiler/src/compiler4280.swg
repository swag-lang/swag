#test
{
    #[Swag.ConstExpr]
    func getType(str: bool)->typeinfo
    {
        if str return string
        else   return s32
    }

    var x: @mktype(getType(false))
    #assert @typeof(x) == s32
    x = 0

    var x1: @mktype(getType(true))
    #assert @typeof(x1) == string
    x1 = "0"
}