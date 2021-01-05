#global if core.ASSERT
#global testerror
#import "core"

using core.sync

#test
{
    mtx := Mutex.create()
    mtx = Mutex.create()
}