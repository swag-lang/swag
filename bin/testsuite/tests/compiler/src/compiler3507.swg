//#[Swag.PrintBc]
#test
{
    func getInc()->closure()->s32
    {
        x := 10
        return @|x|()->s32 {
            x += 1
            return x
        }
    }

    fct := getInc()
    @assert(fct() == 11)
    @assert(fct() == 12)
    @assert(fct() == 13)
}