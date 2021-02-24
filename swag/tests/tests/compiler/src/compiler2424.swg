func print(f: f32)
    #checkif
    {
        @assert(@isconstexpr(f))
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
