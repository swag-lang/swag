#test
{
    // 'defer' is used to call an expression when the current scope is left.
    // It's purely compile time, so it does not evaluate until the block is left
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
        func releaseResource(resource: *bool) = dref resource = false
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

// defer can also have parameters like defer(err) or defer(noerr)

var g_Defer = 0

func raiseError() throw
{
    throw "error"
}

func testDefer(err: bool) throw
{
    defer(err)      g_Defer += 1    // This will be called in case an error is raised, before exiting
    defer(noerr)    g_Defer += 2    // This will only be called in case an error is not raised
    defer           g_Defer += 3    // This will be called in both cases
    if err
        raiseError()
}

#test
{
    catch testDefer(true)
    @assert(g_Defer == 4)
    catch testDefer(false)
    @assert(g_Defer == 9)
}