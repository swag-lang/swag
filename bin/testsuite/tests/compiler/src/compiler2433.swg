func append()
    where
    {
        @assert(#stringof(#me) == "__compiler2433.append")
        return true
    }
{
}

#run
{
    append()
}