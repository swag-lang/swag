//#[Swag.PrintBc]
#test
{
    func getInc()->closure()->s32
    {
        let x = 10
        return closure|x|()->s32
        {
            x += 1
            return x
        }
    }

    let fct = getInc()
    @assert(fct() == 11)
    @assert(fct() == 12)
    @assert(fct() == 13)
}
