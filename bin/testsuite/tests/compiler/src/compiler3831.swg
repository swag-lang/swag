#[Swag.Optim("", true)]
#test
{
    {
        x := 666
        @init(&x)(555)
        @assert(x == 555)
    }

    {
        x := @[1, 2]
        @init(&x, 2)(555)
        @assert(x[0] == 555)
        @assert(x[1] == 555)
    }
}