macro toto(x: code)
{
    cpt := 0
    loop 10
    {
        cpt += 1
        #insert x
    }
}

//#[swag.printbc]
#test
{
    total := 0
    
    toto()
    {
        total += cpt
    }
    
    @assert(total == 55)
}