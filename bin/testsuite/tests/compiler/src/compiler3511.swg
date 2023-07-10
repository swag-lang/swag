//#[Swag.PrintBc]
#test
{
    #[Swag.Inline]
    func getInc()->closure()->s32
    {
        let x = 10
        let y = 20
        let z = 30
        let fct = closure|x|()->s32 {
            return x
        }

        return fct
    }

    let fct = getInc()
    @assert(fct() == 10)
}