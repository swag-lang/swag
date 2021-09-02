func myFunction(value: bool, parameters: s32...)
{
    @assert(parameters[0] == 10)
    @assert(parameters[1] == 20)
    @assert(parameters[2] == 30)
    @assert(parameters[3] == 40)
}

#test
{
    myFunction(true, 10, 20, 30, 40)
}