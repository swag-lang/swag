#global public
#global namespace debug

#[Swag.inline]
func assert(expr: bool, message: string = null, loc = #callerlocation)
{
    #if ASSERT
    {
        if !expr
        {
            @panic(message, loc)
        }
    }
}
