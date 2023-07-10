#global testerror

#[Swag.Optim("", false)]
#test
{
    {
        let v = 127's8
        let v1: s8 = v + 1
        @assert(v1 == 127)
    }
}