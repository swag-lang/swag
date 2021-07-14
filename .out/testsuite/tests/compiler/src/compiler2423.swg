func print(f: f32)
    #checkif
    {
        if f != 3.14 @errormsg("'f' should be 3.14", #location)
        return true
    }
{

}

#[Swag.ConstExpr]
func tt() => 3.14

#test
{
    x := 3.14
    print(tt())
}
