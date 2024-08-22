/**
# `defer` Statement
`defer` is used to call an expression when the current scope is left. It's purely compile time, so it does not evaluate until the block is left.
*/
#test
{
    var v = 0
    defer @assert(v == 1)
    v += 1
    // defer expression will be executed here
}

/**
# Defer in a Block
`defer` can also be used within a block.
*/
#test
{
    var v = 0
    defer
    {
        v += 10
        @assert(v == 15)
    }

    v += 5
    // defer block will be executed here
}

/**
# Defer with Control Flow
`defer` expressions are called when leaving the corresponding scope, even with `return`, `break`, `continue`, etc., and even inside a `loop/while/for`, etc.
*/
#test
{
    var G = 0
    loop 10
    {
        defer G += 1
        if G == 2:
            break // defer expression will be called here, before breaking the loop
        // defer expression will be called here as well
    }

    @assert(G == 3)
}

/**
# Defer Execution Order
`defer` statements are executed in reverse order of their declaration.
*/
#test
{
    var x = 1
    defer @assert(x == 2) // Will be executed second
    defer x *= 2          // Will be executed first
}

/**
# Typical Usage of `defer`
`defer` is typically used to unregister/destroy a resource, by placing the release code immediately after the creation code.
*/
#test
{
    func createResource()                 => true
    func releaseResource(resource: *bool) = dref resource = false
    func isResourceCreated(b: bool)       => b

    var resource = false
    loop 10
    {
        resource = createResource()
        defer
        {
            @assert(resource.isResourceCreated())
            releaseResource(&resource)
        }

        if #index == 2:
            break
    }

    @assert(!resource.isResourceCreated())
}
