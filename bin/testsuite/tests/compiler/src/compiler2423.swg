func print(f: f32)
    verify
    {
        if f != 3.14 do
            @compilererror("'f' should be 3.14", #curlocation)
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
