#import "core"
using core

#test
{
    var v: swag.SystemAllocator
    system.pushAllocator(v)
    {
        try jobsystem.init(4)
    }
}