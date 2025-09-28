//#global skip
var G = 0
struct(TTT) Soa
{
    #ast
    {
        foreach p in #typeof(TTT).name
        {
            G += 1
        }

        return ""
    }
}

#test
{
    struct Test { x, y, z: f32; }
    var v: Soa'Test
    if @bytecode:
        @assert(G == 4)
}