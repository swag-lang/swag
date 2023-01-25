func print(f: f32)
    #selectif
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
