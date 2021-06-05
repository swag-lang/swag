func print(f: f32)
    #checkif
    {
        @assert(@isconstexpr(f))
        return true
    }
{

}

#[Swag.constexpr]
func tt() => 3.14

#test
{
    x := 3.14
    print(tt())
}
