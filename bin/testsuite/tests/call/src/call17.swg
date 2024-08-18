using test_foreign

#[Swag.Safety("", false)]
#test
{
    var x: tcf_Point

    var t = @typeof(tcf_Point)
    visit p in t.methods
    {
        if p.name != "set" do
            continue

        var f = p.value
        @assert(f != null)

        var fn = cast(func(*tcf_Point)) f
        fn(&x)
        break
    }

    @assert(x.x == 444)
    @assert(x.y == 555)
    @assert(x.z == 666)

    t.opInit(&x)
    /*visit p in t.methods
    {
        if p.name != "opInit"
            continue

        var f = p.value
        @assert(f != null)

        var fn = cast(func(*tcf_Point)) f
        fn(&x)
        break
    }*/

    @assert(x.x == 0)
    @assert(x.y == 0)
    @assert(x.z == 0)
}