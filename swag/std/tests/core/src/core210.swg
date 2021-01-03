#if core.ASSERT
{
    #import "core"
    #unittest error

    using core.sync

    #test
    {
        mtx := Mutex.create()
        mtx = Mutex.create()
    }
}