//#global testerror "Err0573"
struct RGB{a, b, c: s32}

#test
{
    const X: RGB = #run
        {
            return RGB{1, 2, 3}
        }
    #assert X.a == 1
    #assert X.b == 2
    #assert X.c == 3
}