#test
{
    {
        const x, y: *u8 = null
        const z = x > y
        #assert(z == false)
    }

    {
        const x, y: *u8 = null
        const z = x < y
        #assert(z == false)
    }

    {
        const x, y: *u8 = null
        const z = x <=> y
        #assert(z == 0)
    }
}
