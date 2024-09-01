#assert !#defined(TOIKTOI)
#assert #defined(G)
var G = 0

#if #defined(PRRRRR)
{
    #assert(false)
}

#if #defined(Value)
{
    const Value1 = Value
}

const Value = 1
#assert Value1 == 1