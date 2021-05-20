#global if Core.ASSERT
#global testerror


using Core.sync

#test
{
    mtx := Mutex.create()
    mtx = Mutex.create()
}