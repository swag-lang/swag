//#[Swag.PrintBc]
#test
{
    func getInc()->closure()->s32
    {
        x := 10
        y := 20
        z := 30
        return closure|x, z|()->s32 {
            x = x + z + 1
            return x
        }
    }

    fct := getInc()
    @assert(fct() == 41)
    @assert(fct() == 72)
}