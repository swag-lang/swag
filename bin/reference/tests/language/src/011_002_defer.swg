/**
# 'defer' Statement
The 'defer' statement registers an expression that executes automatically
when the current scope is exited. It runs purely at compile time and ensures
that cleanup or finalization logic is always performed. This helps maintain
clarity and safety in resource management.
*/
#test
{
    var v = 0
    defer @assert(v == 1) // Ensures v equals 1 on scope exit.
    v += 1 // Increment v.

    // When the scope ends, the deferred expression executes here.
}

/**
# 'Defer in a Block'
A 'defer' can also enclose multiple expressions within a block. This allows
you to group operations that should all run when leaving the scope.
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
    // Upon scope exit, the defer block executes, ensuring v == 15.
}

/**
# 'Defer with Control Flow'
The deferred expression executes every time a scope exits, even when leaving
via 'return', 'break', or 'continue'. This guarantees proper cleanup in all
control flow paths.
*/
#test
{
    var G = 0
    for 10
    {
        defer G += 1 // Increment G at end of each iteration, even if broken.

        if G == 2 do
            break // Exiting early still triggers the defer expression.
    }

    @assert(G == 3)
}

/**
# 'Defer Execution Order'
When multiple 'defer' statements exist, they execute in reverse declaration
order. The most recent defer runs first when the scope ends.
*/
#test
{
    var x = 1
    defer @assert(x == 2) // Executes second
    defer x *= 2 // Executes first

    // Execution order is reversed for predictable cleanup flow.
}

/**
# Example: Resource Management
'defer' is ideal for resource handling — ensuring that allocation and
release logic stay close together and cleanup always happens.
*/
#test
{
    func createResource()                 => true
    func releaseResource(resource: *bool) { dref resource = false }
    func isResourceCreated(b: bool)       => b

    var resource = false
    for 10
    {
        resource = createResource()
        defer
        {
            @assert(resource.isResourceCreated())
            releaseResource(&resource)
        }

        if @index == 2 do
            break // Defer ensures cleanup even on early exit.
    }

    @assert(!resource.isResourceCreated())
}

/**
# Example: Error Handling
'defer' ensures reliable cleanup even in functions that may return early due
to errors. This pattern creates robust, error-resilient code.
*/
#test
{
    func createResource()                 => true
    func releaseResource(resource: *bool) { dref resource = false }
    func isResourceCreated(b: bool)       => b

    func performTask()->bool
    {
        var resource = createResource()
        defer releaseResource(&resource) // Always release resource.

        if !resource.isResourceCreated()
        {
            // Early return still triggers defer.
            return false
        }

        // Perform work...
        return true
    }

    let success = performTask()
    @assert(success)
}
