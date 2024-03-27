#test
{
    loop 5
    {
        #if false
        {
            #assert false
        }
        #else
        {
            #assert true
        }
    }
}