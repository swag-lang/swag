//#global testerror
func(op: s32, T) toto(v: T)
{
    #if op == 5:
        @assert(v == true)
    #elif op == 10:
        @assert(v == "string")
    #else:
        #assert false
    //@print(op, " ", v, "\n")
}

#test
{
    toto'5(true)
    toto'10("string")
}
