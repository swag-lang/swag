#test
{
    @assert(@getcontext().flags & Swag.ContextFlags.Test ? true : false)
}