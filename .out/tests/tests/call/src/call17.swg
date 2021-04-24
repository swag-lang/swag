using tests_foreign

#[swag.safety("", false)]
#test
{
    var x: tcf_Point

    t := @typeof(tcf_Point)
    visit p: t.methods
    {
        if p.name != "set"
            continue

        f := p.value
        @assert(f != null)

        fn := cast(func(*tcf_Point)) f
        fn(&x)
        break
    }

    @assert(x.x == 444)
    @assert(x.y == 555)
    @assert(x.z == 666)
}