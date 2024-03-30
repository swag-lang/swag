using Core

#test
{
    const C = "aa toto la foule aa toto la foule"
    let a   = Tokenize.split(C, "aa")
    @assert(a.count == 2)
    @assert(a[0] == " toto la foule ")
    @assert(a[1] == " toto la foule")
}
