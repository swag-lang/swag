#[Swag.ConstExpr]
func toto() => 1

#test
{
    @assert(@isconstexpr(1))

    let x = 0
    @assert(!@isconstexpr(x))
    @assert(!@isconstexpr(x + 1))

    @assert(@isconstexpr(toto() + 1))
}
