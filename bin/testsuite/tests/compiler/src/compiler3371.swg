//#global skip
var G = 0
struct(TTT) Soa
{
    #ast
    {
        visit p: @typeof(TTT).flatName
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
    if @isbytecode()
        @assert(G == 4)
}