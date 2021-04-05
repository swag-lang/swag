#test
{
    // If you use 'const' instead of 'var', the value must be known by the compiler
    // (no memory footprint)
    {
        const a = 666
        #assert(a == 666)

        const b: string = "string"
        #assert(b == "string")
    }
}