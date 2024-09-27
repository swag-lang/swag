/**
# `defer` Statement
The `defer` statement allows you to specify an expression that will be automatically executed when the 
current scope is exited. Since `defer` operates purely at compile time, the deferred expression is not 
evaluated until the block of code in which it is defined is left. This feature is useful for managing 
resources, ensuring cleanup operations, and maintaining code clarity.
*/
#test
{
    var v = 0
    defer @assert(v == 1)   // Ensures that v equals 1 when leaving the scope.
    v += 1                  // Increment v by 1.

    // When the scope is exited, the deferred expression will be executed here, verifying that v is 1.
}

/**
# Defer in a Block
The `defer` statement can also encapsulate multiple expressions within a block. This allows you to 
group together operations that should all be executed upon exiting the scope, maintaining the integrity 
of your logic and ensuring that all necessary actions are performed.
*/
#test
{
    var v = 0
    defer
    {
        v += 10            // Increment v by 10.
        @assert(v == 15)   // Ensure v equals 15 after the block execution.
    }

    v += 5                // Increment v by 5.

    // Upon scope exit, the defer block is executed, adding 10 to v and ensuring the final value is 15.
}

/**
# Defer with Control Flow
The `defer` expression is executed whenever the scope is exited, including in the presence of control 
flow statements like `return`, `break`, or `continue`. This behavior makes `defer` particularly 
advantageous for scenarios that require guaranteed execution of specific operations, such as resource 
cleanup or ensuring that certain conditions are met, regardless of how the code block is terminated.
*/
#test
{
    var G = 0
    for 10
    {
        defer G += 1   // Ensure G is incremented each loop iteration, even if the loop is exited early.

        if G == 2:
            break      // Exit the loop when G equals 2. Defer will execute before breaking out.

        // The defer expression is executed at the end of each iteration.
    }

    @assert(G == 3)    // Check that G has been correctly incremented, even after the loop is broken.
}

/**
# Defer Execution Order
When multiple `defer` statements are declared, they are executed in the reverse order of their 
declaration. This ensures that the most recently deferred operation occurs first upon scope exit, 
providing a predictable and manageable flow of operations.
*/
#test
{
    var x = 1
    defer @assert(x == 2)   // Executed second, after x is multiplied by 2.
    defer x *= 2            // Executed first, doubling the value of x.

    // The deferred statements execute in reverse, ensuring logical and predictable order.
}

/**
# Example: Defer for Resource Management
A common use case for `defer` is in resource management, such as the creation and subsequent release 
of resources. By placing the release logic immediately after the creation logic, the code becomes 
more readable and ensures that resources are always properly managed, even in the event of an error or 
early exit.
*/
#test
{
    func createResource()                    => true
    func releaseResource(resource: nn *bool) = dref resource = false
    func isResourceCreated(b: bool)          => b

    var resource = false
    for 10
    {
        resource = createResource()
        defer
        {
            @assert(resource.isResourceCreated())   // Validate that the resource was created.
            releaseResource(&resource)              // Release the resource when exiting the loop.
        }

        if #index == 2:
            break      // Exit for early, defer block still ensures resource release.
    }

    @assert(!resource.isResourceCreated())   // Ensure the resource is no longer created post-loop.
}

/**
# Example: Defer in Error Handling
In more complex functions, `defer` proves invaluable for ensuring that resources are cleaned up 
reliably, even in the presence of errors or early returns. This pattern is essential for writing 
robust, error-resilient code that gracefully handles failure scenarios while ensuring that all necessary 
cleanup is performed.
*/
#test
{
    func createResource()                    => true
    func releaseResource(resource: nn *bool) = dref resource = false
    func isResourceCreated(b: bool)          => b

    func performTask() -> bool
    {
        var resource = createResource()
        defer releaseResource(&resource)   // Guarantee resource release on function exit.

        if !resource.isResourceCreated()
        {
            // Handle error: resource wasn't created. Defer block still ensures cleanup.
            return false
        }

        // Perform other tasks here...
        // If an error occurs, the defer block will release the resource.
        return true
    }

    let success = performTask()
    @assert(success)   // Ensure that the task was successful and resources were managed correctly.
    // The resource is released correctly regardless of the function's outcome.
}
