//#[Swag.PrintBc]
#test
{
    func getInc()->closure()->s32
    {
        x := 10
        y := 20
        z := 30
        fct := closure|x|()->s32 {
            return x
        }

        return fct
    }

    fct := getInc()
    @assert(fct() == 10)
}