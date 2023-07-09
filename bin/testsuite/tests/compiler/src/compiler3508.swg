//#[Swag.PrintBc]
#test
{
    func getInc()->closure()->s32
    {
        var x = 10
        var y = 20
        var z = 30
        return closure|x, z|()->s32 {
            x = x + z + 1
            return x
        }
    }

    var fct = getInc()
    @assert(fct() == 41)
    @assert(fct() == 72)
}