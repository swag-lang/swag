#global skip
struct(TTT) Soa
{
    #ast
    {
        visit p: @typeof(TTT).fields
        {
        }

        return ""
    }
}

#test
{
    struct Test { x, y, z: f32; }
    var v: Soa'Test
}