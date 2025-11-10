enum A: const [..] s32
{
    BB = null
}

//#global skip
//#[Swag.PrintBc]
#test
{
    const x = A.BB
    #assert(x == null)
}
