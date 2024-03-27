#test
{
    let cpt = 4
    loop cpt
    {
        #scope up
        {
            @assert(#index == 0)
            return
        }
    }
}
