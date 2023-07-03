//#[Swag.PrintBc, Swag.Optim("", false)]
#test
{
    x := {a = 2, b = 3}

    v := closure|x|()
    {
        @assert(x.item0 == 2)
        @assert(x.item1 == 3)
        @assert(x.a == 2)
        @assert(x.b == 3)
    }

    v()
}