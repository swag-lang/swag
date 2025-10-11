using Core

#test
{
    let str: String = "ABC"
    var cpt  = 0
    foreach #Bytes #reverse v in str
    {
        if cpt == 0 do
            @assert(v == 'C')
        if cpt == 1 do
            @assert(v == 'B')
        if cpt == 2 do
            @assert(v == 'A')
        cpt += 1
    }
}
