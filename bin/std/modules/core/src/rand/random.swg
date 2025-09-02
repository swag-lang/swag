#global namespace Random

#[Swag.Tls]
internal var g_SharedRng: Rng'MWC

// Get the default per thread random generator
public func shared()->*Rng'MWC => &g_SharedRng
