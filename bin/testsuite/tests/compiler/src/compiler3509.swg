//#[Swag.PrintBc]
#test
{
    func getInc()->closure()->s32
    {
        var x = 10
        var y = 20
        var z = 30
        var fct = closure|x|()->s32 {
            return x
        }

        return fct
    }

    var fct = getInc()
    @assert(fct() == 10)
}