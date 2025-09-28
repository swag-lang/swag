using Core, Win32

// Represents a bus.
// A [[Voice]] can be assigned to one or more buses. If you then change some parameters of the bus (like the volume), then all the voices assigned to it will be impacted.
public struct Bus
{
    handle: BusHandle
}

public impl Bus
{
    // Creates an audio bus.
    // You can then associate a [[Voice]] to that bus with [[Voice.setRooting]]
    // Note that you can have a graph of buses, because a bus can have another bus as 'parent'.
    func create(numChannels: u32, parent: *Bus = null)->*Bus throw
    {
        let bus = Memory.new'Bus()
        defer #err Memory.delete(bus)
        createNative(bus, numChannels, parent)

        Sync.scopedLock(&g_Driver.mutexBuses)
        g_Driver.buses.add(bus)
        return bus
    }

    // Destroy the bus (immediatly).
    mtd destroy() throw
    {
        .destroyNative()

        Sync.scopedLock(&g_Driver.mutexBuses)
        g_Driver.buses.remove(me)
        Memory.delete(me)
    }

    // Set the playing bus volume in DB.
    mtd setVolumeDB(volumeDB: f32, batchID: u32 = 0) throw
    {
        .setVolume(convertDBToPercent(volumeDB), batchID)
    }

    // Returns the actual volume, in DB.
    mtd getVolumeDB()->f32 throw
    {
        return convertPercentToDB(.getVolume())
    }
}
