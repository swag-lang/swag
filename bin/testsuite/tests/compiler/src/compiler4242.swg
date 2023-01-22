#global skip
#[Swag.ConstExpr]
func toto()->typeinfo => @typeof(s32)

#test
{
    #if toto() == s32
    {
    }
}