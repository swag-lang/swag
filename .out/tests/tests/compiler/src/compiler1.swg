#test
{
    #[swag.enumflags]
    enum V { A; B; }
    x := V.A | V.B
    @assert(x & .A && x.B)
}