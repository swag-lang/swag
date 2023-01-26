#test
{
    #[Swag.ConstExpr]
    func getType(str: bool) => str ? string : s32

    var x: @mktype(getType(false))
    #assert @typeof(x) == s32
    x = 0

    var x1: @mktype(getType(true))
    #assert @typeof(x1) == string
    x1 = "0"
}