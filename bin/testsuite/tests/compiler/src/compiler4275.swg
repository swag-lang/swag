#global skip

var G = 0
/*
#[Swag.Macro]
func toto(#[Swag.ConstExpr] str: string)
    #selectif @isconstexpr(str)
{
    G += 1
    #[Swag.SelectIf(false)]
    toto(str)
}*/

#[Swag.PrintBc]
func toto(str: string)
    #selectif
    {
        if !@isconstexpr(str)
            return true
        @compilererror("toto", @location(str))
        return true
    }
{
    G += 10
}

#test
{
    toto("a")
}