#global marked
#global marked
#global export
#global namespace Serialization
using Swag

interface IDecoder
{
    mtd getVersion()->u32;
    mtd isTextual()->bool;

    mtd start(input: const [..] u8) throw;
    mtd end() throw;

    mtd beginField(val: Swag.TypedValue)->bool throw;
    mtd endField(val: Swag.TypedValue) throw;
    mtd beginSection(kind: SectionKind) throw;
    mtd endSection() throw;

    mtd read(data: *void, dataType: typeinfo) throw;
    mtd readBufferU8(count: u64)->*u8 throw;

    mtd readBool()->bool throw;
    mtd readS8()->s8 throw;
    mtd readS16()->s16 throw;
    mtd readS32()->s32 throw;
    mtd readS64()->s64 throw;
    mtd readU8()->u8 throw;
    mtd readU16()->u16 throw;
    mtd readU32()->u32 throw;
    mtd readU64()->u64 throw;
    mtd readF32()->f32 throw;
    mtd readF64()->f64 throw;
    mtd readString()->String throw;
}

#[Swag.EnumFlags]
enum DecoderFlags
{
    Zero
    IgnoreStructFieldError
}

/*
Serialization decoder.
This is a high level decoder that needs a specific implementation like [[Core.Serialization.Read.TagBin]] or [[Core.Serialization.Read.JSon]].

```swag
var res = File.readAllBytes(file)
var decoder: Decoder'Read.TagBin
let myStruct = catch decoder.readAll'MyStruct(res.toSlice())
```
*/
struct(T) Decoder
{
    serializer:     T
    flags:          DecoderFlags = Zero
}

impl Decoder
{
    mtd readTypeValue(typedVal: TypedValue, data: ^void) throw
    {
        discard me.serializer.beginElement(typedVal)
        me.readTypedContent(data, typedVal.pointedType)
        me.serializer.endElement(typedVal)
    }

    mtd readTypedContent(data: ^void, dataType: typeinfo) throw
    {
        var type = Reflection.makeConcreteAlias(dataType)
        type = Reflection.makeConcreteEnum(type)

        me.serializer.beginValue()
        defer #noerr me.serializer.endValue()

        // Native
        if type.kind == .Native
        {
            let typeNative = cast(const *TypeInfoNative) type
            switch typeNative.nativeKind
            {
            case Bool:
                dref cast(*bool) data = me.serializer.readNative'bool()
            case U8:
                dref cast(*u8) data = me.serializer.readNative'u8()
            case U16:
                dref cast(*u16) data = me.serializer.readNative'u16()
            case U32, Rune:
                dref cast(*u32) data = me.serializer.readNative'u32()
            case U64:
                dref cast(*u64) data = me.serializer.readNative'u64()
            case S8:
                dref cast(*s8) data = me.serializer.readNative's8()
            case S16:
                dref cast(*s16) data = me.serializer.readNative's16()
            case S32:
                dref cast(*s32) data = me.serializer.readNative's32()
            case S64:
                dref cast(*s64) data = me.serializer.readNative's64()
            case F32:
                dref cast(*f32) data = me.serializer.readNative'f32()
            case F64:
                dref cast(*f64) data = me.serializer.readNative'f64()
            }

            return
        }

        if Reflection.isType(type)
        {
            let crc      = me.serializer.readNative'u32()
            let typeType = Reflection.crcToType(crc)
            dref cast(*typeinfo) data = typeType
            return
        }

        if Reflection.isInterface(type)
        {
            let itf = cast(*Swag.Interface) data

            me.serializer.beginSection(.Raw)
            defer #noerr me.serializer.endSection()

            var objType, itfType: typeinfo
            var typeVal: TypedValue
            typeVal.pointedType = typeinfo

            me.readTypeValue(typeVal, &objType)

            if objType
            {
                me.readTypeValue(typeVal, &itfType)

                itf.itable = @tableof(cast(const *Swag.TypeInfoStruct) objType, cast(const *Swag.TypeInfoStruct) itfType)

                typeVal.pointedType = objType
                itf.obj             = Memory.new(objType)
                me.readTypeValue(typeVal, itf.obj)
            }

            return
        }

        if Reflection.isStruct(type)
        {
            // Core.String
            if type == String
            {
                let strBuf = cast(*String) data
                dref strBuf = #nodrop me.serializer.readNative'String()
                return
            }

            // Core.Array
            let valueType = Reflection.getStructArrayType(type)
            if valueType
            {
                var count   = 0'u64
                let treated = me.serializer.beginSequence(type, data, &count)
                defer #noerr me.serializer.endSequence()

                if !treated
                {
                    Reflection.clearStructArray(data, valueType)
                    while me.serializer.toNextSequenceElement(&count)
                    {
                        let bufArr = Reflection.appendValueStructArray(data, valueType)
                        me.readTypedContent(bufArr, valueType)
                    }
                }

                return
            }

            // A structure with an implementation of ISerialize
            let idecoder = cast(IDecoder) me
            let itf      = @mkinterface(data, type, ISerialize)
            if itf != null
            {
                if itf.read(idecoder):
                    return
            }

            // Generic struct load
            let structType = cast(const *TypeInfoStruct) type
            let treated    = me.serializer.beginStruct(structType, data)
            defer #noerr me.serializer.endStruct()
            if treated:
                return

            foreach field in structType.fields
            {
                if Reflection.hasAttribute(field, NoSerialize) or Reflection.hasAttribute(field.pointedType, NoSerialize):
                    continue

                if !itf or !itf.readElement(field, cast(^u8) data + field.offset, idecoder)
                {
                    let doIt = catch me.serializer.beginElement(field)
                    defer #noerr me.serializer.endElement(field)

                    if @err != null and !(me.flags & .IgnoreStructFieldError):
                        throw @err
                    if !doIt:
                        continue

                    catch me.readTypedContent(cast(^u8) data + field.offset, field.pointedType)
                    if @err != null and !(me.flags & .IgnoreStructFieldError):
                        throw @err
                }
            }

            // Inform user code we are done
            if itf:
                itf.postRead(data, idecoder)

            return
        }

        // Static array
        if Reflection.isStaticArray(type)
        {
            let typeArr = cast(const *TypeInfoArray) type
            var count   = typeArr.count
            let treated = me.serializer.beginSequence(typeArr.pointedType, data, &count)
            defer #noerr me.serializer.endSequence()
            if treated:
                return

            var bufArr = cast(^u8) data
            for count
            {
                me.readTypedContent(bufArr, typeArr.pointedType)
                bufArr += typeArr.pointedType.sizeof
            }

            return
        }
    }

    // Start reading
    mtd start(input: const [..] u8) throw
    {
        me.serializer.startRead(input)
        me.serializer.beginRoot()
    }

    // Finish reading
    mtd end() throw
    {
        me.serializer.endRoot()
    }

    // Read a full struct
    mtd(TR) readAll(input: const [..] u8)->*TR throw
    {
        Debug.assert(Reflection.isStruct(TR))

        me.start(input)

        let val = Memory.new'TR()
        defer #err Memory.delete(val)

        me.readTypedContent(val, TR)

        me.end()
        return val
    }
}

impl IDecoder for Decoder
{
    mtd impl start(input: const [..] u8) throw
    {
        me.serializer.startRead(input)
        me.serializer.beginRoot()
    }

    mtd impl end() throw
    {
        me.serializer.endRoot()
    }

    mtd impl beginField(val: Swag.TypedValue)->bool throw
    {
        if !me.serializer.beginElement(val):
            return false
        me.serializer.beginValue()
        return true
    }

    mtd impl endField(val: Swag.TypedValue) throw
    {
        me.serializer.endValue()
        me.serializer.endElement(val)
    }

    mtd impl beginSection(kind: SectionKind) throw
    {
        me.serializer.beginSection(kind)
    }

    mtd impl endSection() throw
    {
        me.serializer.endSection()
    }

    mtd impl getVersion()->u32
    {
        return me.serializer.getVersion()
    }

    mtd impl isTextual()->bool
    {
        return me.serializer.isTextual()
    }

    mtd impl read(data: *void, dataType: typeinfo) throw
    {
        me.readTypedContent(data, dataType)
    }

    mtd impl readBufferU8(count: u64)->*u8 throw
    {
        let data = cast(^u8) Memory.alloc(count)

        var cpt     = count
        let treated = me.serializer.beginSequence(u8, data, &cpt)
        defer #noerr me.serializer.endSequence()

        if !treated
        {
            var ptr = data
            for Math.min(cpt, count)
            {
                me.serializer.beginValue()
                dref ptr = me.serializer.readNative'u8()
                me.serializer.endValue()
                ptr += 1
            }
        }

        return data
    }

    mtd impl readBool()->bool throw     => me.serializer.readNative'bool()
    mtd impl readS8()->s8 throw         => me.serializer.readNative's8()
    mtd impl readS16()->s16 throw       => me.serializer.readNative's16()
    mtd impl readS32()->s32 throw       => me.serializer.readNative's32()
    mtd impl readS64()->s64 throw       => me.serializer.readNative's64()
    mtd impl readU8()->u8 throw         => me.serializer.readNative'u8()
    mtd impl readU16()->u16 throw       => me.serializer.readNative'u16()
    mtd impl readU32()->u32 throw       => me.serializer.readNative'u32()
    mtd impl readU64()->u64 throw       => me.serializer.readNative'u64()
    mtd impl readF32()->f32 throw       => me.serializer.readNative'f32()
    mtd impl readF64()->f64 throw       => me.serializer.readNative'f64()
    mtd impl readString()->String throw => me.serializer.readNative'String()
}
