#global if core.ASSERT
#global testerror


using core.sync

#test
{
    mtx := Mutex.create()
    mtx = Mutex.create()
}