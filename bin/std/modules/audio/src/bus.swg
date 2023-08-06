using Core, Win32

// Represents a playing sound
public struct Bus
{
    handle: BusHandle
}

public impl Bus
{
    // Creates an audio bus
    func create(numChannels: u32, parent: *Bus = null)->*Bus throw
    {
        let bus = Memory.new'Bus()
        defer(err) Memory.delete(bus)
        createNative(bus, numChannels, parent)

        Sync.scopedLock(&g_Driver.mutexBuses)
        g_Driver.buses.add(bus)
        return bus
    }

    // Creates an audio bus
    mtd destroy() throw
    {
        destroyNative()

        Sync.scopedLock(&g_Driver.mutexBuses)
        g_Driver.buses.remove(self)
        Memory.delete(self)
    }
}