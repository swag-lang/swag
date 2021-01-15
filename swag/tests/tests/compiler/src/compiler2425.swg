#[swag.constexpr]
func toto() => 1

#[swag.printbc]
#test
{
    @assert(@isconstexpr(1))

    x := 0
    @assert(!@isconstexpr(x))
    @assert(!@isconstexpr(x + 1))

    @assert(@isconstexpr(toto() + 1))
}
