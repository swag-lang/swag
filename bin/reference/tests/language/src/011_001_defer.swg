/**
# `defer` Statement
`defer` is used to call an expression when the current scope is left. It's purely compile time, so it does not evaluate until the block is left.
*/
#test
{
    var v = 0
    defer @assert(v == 1)
    v += 1
    // The defer expression will be executed here, after v is incremented to 1.
    // This allows you to ensure that certain operations are performed when leaving the scope.
}

/**
# Defer in a Block
`defer` can also be used within a block, allowing you to group multiple statements together that should be executed when leaving the scope.
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
    // The defer block will be executed here, ensuring that v is correctly incremented by 10 after the main logic.
}

/**
# Defer with Control Flow
`defer` expressions are executed when leaving the corresponding scope, even with control flow statements like `return`, `break`, or `continue`. This makes `defer` particularly useful for cleaning up resources or ensuring certain actions are performed, regardless of how the scope is exited.
*/
#test
{
    var G = 0
    loop 10
    {
        defer G += 1  // This defer ensures that G is incremented every time the loop iterates, even if we break out early.
        if G == 2:
            break // defer expression will be executed here before breaking the loop.
        // defer expression will be executed here as well.
    }

    @assert(G == 3)  // Verifies that G was incremented correctly, even after breaking out of the loop.
}

/**
# Defer Execution Order
`defer` statements are executed in reverse order of their declaration. This means that the last `defer` statement you declare will be the first one to execute when the scope is exited.
*/
#test
{
    var x = 1
    defer @assert(x == 2) // This will be executed second, after x is multiplied by 2.
    defer x *= 2          // This will be executed first, doubling the value of x.
}

/**
# Example: Defer for Resource Management
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

/**
# Example: Defer in Error Handling
In complex functions, `defer` can help ensure that resources are always cleaned up, even in the presence of errors. This pattern is essential for writing robust, error-resilient code.
*/
#test
{
    func createResource()                 => true
    func releaseResource(resource: *bool) = dref resource = false
    func isResourceCreated(b: bool)       => b

    func performTask() -> bool
    {
        var resource = createResource()
        defer
        {
            releaseResource(&resource)
        }

        if !resource.isResourceCreated()
        {
            // Return early if the resource wasn't created, but the defer block will still run.
            return false
        }

        // Perform other tasks...
        // If an error occurs, the resource will still be released.
        return true
    }

    let success = performTask()
    @assert(success)
    // No matter what happened in performTask, the resource was released correctly.
}