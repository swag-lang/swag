//#[Swag.PrintBc]
#test
{
    func getInc()->closure()->s32
    {
        let x = 10
        let y = 20
        let z = 30
        return closure|x, z|()->s32 {
            x = x + z + 1
            return x
        }
    }

    let fct = getInc()
    @assert(fct() == 41)
    @assert(fct() == 72)
}