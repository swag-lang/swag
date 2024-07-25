func print(f: f32)
    where:call
    {
        if f != 3.14 do @compilererror("'f' should be 3.14", #location)
        return true
    }
{

}

#[Swag.ConstExpr]
func tt() => 3.14

#test
{
    var x = 3.14
    print(tt())
}
