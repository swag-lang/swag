#test
{
    @assert(false and false or true == true)
    @assert((false and (false or true)) == false)
    @assert(((false and false) or true) == true)
}
