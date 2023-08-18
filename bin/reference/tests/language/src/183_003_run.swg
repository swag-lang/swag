/**
`#run` is a special function that will be called at **compile time**.
It can be used to precompute some global values for example.
*/
var G: [5] f32 = undefined

/**
Initialize `G` with `[1,2,4,8,16]` at compile time.
*/
#run
{
    var value = 1'f32
    loop i: @countof(G)
    {
        G[i] = value
        value *= 2
    }
}

/**
`#test` are executed after `#run`, even at compile time (during testing).
So we can test the values of `G` here.
*/
#test
{
    @assert(G[0] == 1)
    @assert(G[1] == 2)
    @assert(G[2] == 4)
    @assert(G[3] == 8)
    @assert(G[4] == 16)
}

/**
`#run` can also be used as an expression, to call for example a function not marked with `#[Swag.ConstExpr]`.
*/
const SumValue = #run sum(1, 2, 3, 4) + 10
#assert SumValue == 20

func sum(values: s32...)->s32
{
    var result = 0's32
    visit v: values
        result += v
    return result
}

/**
`#run` can also be used as an expression block.
The return type is deduced from the `return` statement.
*/
const Value = #run {
    var result: f32
    loop 10
        result += 1
    return result   // 'Value' will be of type 'f32'
}
#assert Value == 10.0