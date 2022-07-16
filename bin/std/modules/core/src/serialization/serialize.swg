#global namespace Serialization

public interface ISerialize
{
    read:  func(self, IDecoder) throw
    write: func(self, IEncoder)
}