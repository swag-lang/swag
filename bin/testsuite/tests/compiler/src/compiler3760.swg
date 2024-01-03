#global testerror "Saf0009"

#[Swag.Optim("", false)]
#[Swag.Safety("", true)]
#test
{
    {
        let v = 127's8
        let v1: s8 = v + 1
        @assert(v1 == 127)
    }
}