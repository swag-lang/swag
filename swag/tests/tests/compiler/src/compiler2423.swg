func print(f: f32)
    #selectif
    {
        if f != 3.14 @errormsg("'f' should be 3.14", #location)
        return true
    }
{

}

#[swag.constexpr]
func tt() => 3.14

#test
{
    x := 3.14
    print(tt())
}
