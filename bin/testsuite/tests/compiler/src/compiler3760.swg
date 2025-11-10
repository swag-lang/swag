#global testerror "Saf0027"

#[Swag.Optimize(.All, false), Swag.Safety(.All, .All, true)]
#test
{
    {
        let v   = 127's8
        let v1: s8 = v + 1
        @assert(v1 == 127)
    }
}
