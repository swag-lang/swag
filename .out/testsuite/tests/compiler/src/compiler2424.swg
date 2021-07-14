func print(f: f32)
    #checkif
    {
        @assert(@isconstexpr(f))
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
