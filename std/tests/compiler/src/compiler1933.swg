#test
{
    @assert(@getcontext().flags & swag.ContextFlags.Test ? true : false)
}