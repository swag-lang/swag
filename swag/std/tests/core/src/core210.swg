#unittest error

#import "core"
using core.sync

#test
{
    mtx := Mutex.create()
    mtx = Mutex.create()
}
