//#[Swag.PrintBc, Swag.Optim("", false)]
#test
{
    var x: const [..] s32 = [1, 2]

    var v = closure|x|()
    {
        @assert(x[0] == 1)
        @assert(x[1] == 2)
    }

    v()
}