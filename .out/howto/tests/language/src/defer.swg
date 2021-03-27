#test
{
    @dbgbreak()

    // defer is used to call an expression when the current scope is left
    // it's purely compile time, so it does not evaluate until the block is self
    {
        v := 0
        defer @assert(v == 1)
        v += 1
        // defer expression will be executed here
    }

    // defer can also be used with a block
    {
        v := 0
        defer
        {
            v += 10
            @assert(v == 15)
        }

        v += 5
        // defer block will be executed here
    }

    // defer expressions are called when leaving the corresponding scope, even
    // with return, break, continue etc., and even inside a loop/while/for etc.
    {
        var G = 0
        loop 10
        {
            defer G += 1
            if G == 2
                break   // will be called here, before breaking the loop
            // will be called here also
        }

        @assert(G == 3)
    }

    // It's typically used to unregister/destroy a resource, by putting the release
    // code just after the creation one
    {
        func createResource() => true
        func releaseResource(resource: *bool) = :resource = false
        func isResourceCreated(b: bool) => b

        resource := false
        loop 10
        {
            resource = createResource()
            defer
            {
                @assert(resource.isResourceCreated())
                releaseResource(&resource)
            }

            if @index == 2
                break
        }

        @assert(!resource.isResourceCreated())
    }
}