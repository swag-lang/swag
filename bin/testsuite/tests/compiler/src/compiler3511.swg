//#[Swag.PrintBc]
#test
{
    #[Swag.Inline]
    func getInc()->closure()->s32
    {
        x := 10
        y := 20
        z := 30
        fct := @|x|()->s32 {
            return x
        }

        return fct
    }

    fct := getInc()
    @assert(fct() == 10)
}