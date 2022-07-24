#global testerror

#[Swag.Optim("", false)]
#test
{
    {
        v := 127's8
        var v1: s8 = v + 1
        @assert(v1 == 127)
    }
}