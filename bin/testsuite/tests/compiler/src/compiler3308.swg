//#global testerror
func(op: s32, T) toto(v: T)
{
    #if op == 5 do
        @assert(v == true)
    #elif op == 10 do
        @assert(v == "string")
    #else do
        #assert(false)
    //@print(op, " ", v, "\n")
}

#test
{
    toto'5(true)
    toto'10("string")
}
