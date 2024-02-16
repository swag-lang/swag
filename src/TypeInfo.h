#pragma once
#include "Assert.h"
#include "Attribute.h"
#include "CallConv.h"
#include "Mutex.h"
#include "RaceCondition.h"
#include "Runtime.h"
#include "TypeMatch.h"

struct AstFuncDecl;
struct AstNode;
struct BadSignatureInfos;
struct ByteCode;
struct ComputedValue;
struct Job;
struct JobContext;
struct Scope;
struct SemanticContext;
struct SymbolMatchContext;
struct SymbolOverload;
struct TypeInfo;
struct TypeInfoFuncAttr;
struct TypeInfoParam;
struct TypeInfoStruct;

constexpr int COMPUTE_NAME               = 0;
constexpr int COMPUTE_SCOPED_NAME        = 1;
constexpr int COMPUTE_SCOPED_NAME_EXPORT = 2;
constexpr int COMPUTE_DISPLAY_NAME       = 3;

constexpr uint64_t TYPEINFO_SELF                     = 0x00000000'00000001;
constexpr uint64_t TYPEINFO_UNTYPED_BIN_HEX          = 0x00000000'00000002;
constexpr uint64_t TYPEINFO_INTEGER                  = 0x00000000'00000004;
constexpr uint64_t TYPEINFO_FLOAT                    = 0x00000000'00000008;
constexpr uint64_t TYPEINFO_UNSIGNED                 = 0x00000000'00000010;
constexpr uint64_t TYPEINFO_CONST                    = 0x00000000'00000020;
constexpr uint64_t TYPEINFO_AUTO_NAME                = 0x00000000'00000040;
constexpr uint64_t TYPEINFO_VARIADIC                 = 0x00000000'00000080;
constexpr uint64_t TYPEINFO_STRUCT_HAS_INIT_VALUES   = 0x00000000'00000100;
constexpr uint64_t TYPEINFO_STRUCT_ALL_UNINITIALIZED = 0x00000000'00000200;
constexpr uint64_t TYPEINFO_STRUCT_NO_POST_COPY      = 0x00000000'00000400;
constexpr uint64_t TYPEINFO_STRUCT_NO_POST_MOVE      = 0x00000000'00000800;
constexpr uint64_t TYPEINFO_STRUCT_NO_DROP           = 0x00000000'00001000;
constexpr uint64_t TYPEINFO_GENERIC                  = 0x00000000'00002000;
constexpr uint64_t TYPEINFO_POINTER_MOVE_REF         = 0x00000000'00004000;
constexpr uint64_t TYPEINFO_UNTYPED_INTEGER          = 0x00000000'00008000;
constexpr uint64_t TYPEINFO_UNTYPED_FLOAT            = 0x00000000'00010000;
constexpr uint64_t TYPEINFO_DEFINED_VALUE            = 0x00000000'00020000;
constexpr uint64_t TYPEINFO_AUTO_CAST                = 0x00000000'00040000;
constexpr uint64_t TYPEINFO_TYPED_VARIADIC           = 0x00000000'00080000;
constexpr uint64_t TYPEINFO_STRUCT_TYPEINFO          = 0x00000000'00100000;
constexpr uint64_t TYPEINFO_STRUCT_IS_TUPLE          = 0x00000000'00200000;
constexpr uint64_t TYPEINFO_ENUM_FLAGS               = 0x00000000'00400000;
constexpr uint64_t TYPEINFO_SPREAD                   = 0x00000000'00800000;
constexpr uint64_t TYPEINFO_UNDEFINED                = 0x00000000'01000000;
constexpr uint64_t TYPEINFO_ENUM_INDEX               = 0x00000000'02000000;
constexpr uint64_t TYPEINFO_STRICT                   = 0x00000000'04000000;
constexpr uint64_t TYPEINFO_CONST_ALIAS              = 0x00000000'08000000;
constexpr uint64_t TYPEINFO_HAS_USING                = 0x00000000'10000000;
constexpr uint64_t TYPEINFO_FUNC_IS_ATTR             = 0x00000000'20000000;
constexpr uint64_t TYPEINFO_FROM_GENERIC             = 0x00000000'40000000;
constexpr uint64_t TYPEINFO_STRUCT_NO_COPY           = 0x00000000'80000000;
constexpr uint64_t TYPEINFO_CAN_THROW                = 0x00000001'00000000;
constexpr uint64_t TYPEINFO_GENERIC_COUNT            = 0x00000002'00000000;
constexpr uint64_t TYPEINFO_POINTER_ACCEPT_MOVE_REF  = 0x00000004'00000000;
constexpr uint64_t TYPEINFO_C_STRING                 = 0x00000008'00000000;
constexpr uint64_t TYPEINFO_C_VARIADIC               = 0x00000010'00000000;
constexpr uint64_t TYPEINFO_GENERATED_TUPLE          = 0x00000020'00000000;
constexpr uint64_t TYPEINFO_CLOSURE                  = 0x00000040'00000000;
constexpr uint64_t TYPEINFO_INCOMPLETE               = 0x00000080'00000000;
constexpr uint64_t TYPEINFO_STRUCT_IS_ITABLE         = 0x00000100'00000000;
constexpr uint64_t TYPEINFO_CAN_PROMOTE_816          = 0x00000200'00000000;
constexpr uint64_t TYPEINFO_POINTER_ARITHMETIC       = 0x00000400'00000000;
constexpr uint64_t TYPEINFO_LIST_ARRAY_ARRAY         = 0x00000800'00000000;
constexpr uint64_t TYPEINFO_POINTER_REF              = 0x00001000'00000000;
constexpr uint64_t TYPEINFO_POINTER_AUTO_REF         = 0x00002000'00000000;
constexpr uint64_t TYPEINFO_STRUCT_NO_INIT           = 0x00004000'00000000;
constexpr uint64_t TYPEINFO_SPEC_OP_GENERATED        = 0x00008000'00000000;
constexpr uint64_t TYPEINFO_STRUCT_EMPTY             = 0x00010000'00000000;
constexpr uint64_t TYPEINFO_CHARACTER                = 0x00020000'00000000;
constexpr uint64_t TYPEINFO_ENUM_HAS_USING           = 0x00040000'00000000;
constexpr uint64_t TYPEINFO_GHOST_TUPLE              = 0x00080000'00000000;
constexpr uint64_t TYPEINFO_GENERATED_OP_EQUALS      = 0x00100000'00000000;

constexpr uint8_t TYPEINFOPARAM_DEFINED_VALUE    = 0x00000001;
constexpr uint8_t TYPEINFOPARAM_HAS_USING        = 0x00000002;
constexpr uint8_t TYPEINFOPARAM_AUTO_NAME        = 0x00000004;
constexpr uint8_t TYPEINFOPARAM_GENERIC_TYPE     = 0x00000008;
constexpr uint8_t TYPEINFOPARAM_GENERIC_CONSTANT = 0x00000010;
constexpr uint8_t TYPEINFOPARAM_FROM_GENERIC     = 0x00000020;

struct TypeInfo
{
	TypeInfo()          = default;
	virtual ~TypeInfo() = default;

	explicit TypeInfo(const char* name, TypeInfoKind kind)
		: name{name}, kind{kind}
	{
	}

	explicit TypeInfo(TypeInfoKind kind)
		: kind{kind}
	{
	}

	bool isPointerTo(NativeTypeKind pointerKind);
	bool isPointerTo(TypeInfoKind pointerKind);
	bool isPointerTo(const TypeInfo* finalType);
	bool isPointerNull() const;
	bool isPointerVoid();
	bool isPointerConstVoid();
	bool isPointerToTypeInfo() const;
	bool isInitializerList() const;
	bool isArrayOfStruct() const;
	bool isArrayOfEnum() const;
	bool isMethod() const;

	TypeInfo*       getFinalType();
	TypeInfoStruct* getStructOrPointedStruct() const;
	TypeInfo*       getConstAlias();
	const TypeInfo* getConstAlias() const;
	TypeInfo*       getConcreteAlias() const;

	bool hasFlag(uint64_t fl) const { return flags & fl; }
	void addFlag(uint64_t fl) { flags |= fl; }
	void removeFlag(uint64_t fl) { flags &= ~fl; }

	bool isSlice() const { return kind == TypeInfoKind::Slice; }
	bool isInterface() const { return kind == TypeInfoKind::Interface; }
	bool isStruct() const { return kind == TypeInfoKind::Struct; }
	bool isPointer() const { return kind == TypeInfoKind::Pointer; }
	bool isNative() const { return kind == TypeInfoKind::Native; }
	bool isArray() const { return kind == TypeInfoKind::Array; }
	bool isEnum() const { return kind == TypeInfoKind::Enum; }
	bool isAlias() const { return kind == TypeInfoKind::Alias; }
	bool isFuncAttr() const { return kind == TypeInfoKind::FuncAttr; }
	bool isListArray() const { return kind == TypeInfoKind::TypeListArray; }
	bool isListTuple() const { return kind == TypeInfoKind::TypeListTuple; }
	bool isVariadic() const { return kind == TypeInfoKind::Variadic; }
	bool isTypedVariadic() const { return kind == TypeInfoKind::TypedVariadic; }
	bool isCVariadic() const { return kind == TypeInfoKind::CVariadic; }
	bool isCode() const { return kind == TypeInfoKind::Code; }
	bool isLambdaClosure() const { return kind == TypeInfoKind::LambdaClosure; }
	bool isKindGeneric() const { return kind == TypeInfoKind::Generic; }
	bool isNative(NativeTypeKind native) const { return (kind == TypeInfoKind::Native) && (nativeType == native); }
	bool isAny() const { return isNative(NativeTypeKind::Any); }
	bool isString() const { return isNative(NativeTypeKind::String); }
	bool isBool() const { return isNative(NativeTypeKind::Bool); }
	bool isRune() const { return isNative(NativeTypeKind::Rune); }
	bool isVoid() const { return isNative(NativeTypeKind::Void); }
	bool isUndefined() const { return isNative(NativeTypeKind::Undefined); }
	bool isCString() const { return (kind == TypeInfoKind::Pointer) && (flags & TYPEINFO_C_STRING); }
	bool isLambda() const { return (kind == TypeInfoKind::LambdaClosure) && !isClosure(); }
	bool isClosure() const { return (flags & TYPEINFO_CLOSURE); }
	bool isNativeInteger() const { return (flags & TYPEINFO_INTEGER); }
	bool isNativeIntegerUnsignedOrRune() const { return ((flags & TYPEINFO_INTEGER) && (flags & TYPEINFO_UNSIGNED)) || isRune(); }
	bool isNativeIntegerUnsigned() const { return (flags & TYPEINFO_INTEGER) && (flags & TYPEINFO_UNSIGNED); }
	bool isNativeIntegerSigned() const { return (flags & TYPEINFO_INTEGER) && !(flags & TYPEINFO_UNSIGNED); }
	bool isNativeIntegerOrRune() const { return (flags & TYPEINFO_INTEGER) || isRune(); }
	bool isNativeFloat() const { return (flags & TYPEINFO_FLOAT); }
	bool isConst() const { return (flags & TYPEINFO_CONST); }
	bool isStrict() const { return (flags & TYPEINFO_STRICT); }
	bool isGeneric() const { return (flags & TYPEINFO_GENERIC); }
	bool isFromGeneric() const { return (flags & TYPEINFO_FROM_GENERIC); }
	bool isTuple() const { return (flags & TYPEINFO_STRUCT_IS_TUPLE); }
	bool isPointerRef() const { return (flags & TYPEINFO_POINTER_REF); }
	bool isPointerMoveRef() const { return (flags & TYPEINFO_POINTER_REF) && (flags & TYPEINFO_POINTER_MOVE_REF); }
	bool isConstPointerRef() const { return (flags & TYPEINFO_POINTER_REF) && (flags & TYPEINFO_CONST); }
	bool isAutoConstPointerRef() const { return (flags & TYPEINFO_POINTER_REF) && (flags & TYPEINFO_CONST) && (flags & TYPEINFO_POINTER_AUTO_REF); }
	bool isPointerArithmetic() const { return (flags & TYPEINFO_POINTER_ARITHMETIC); }
	bool isSelf() const { return (flags & TYPEINFO_SELF); }
	bool isUntypedInteger() const { return (flags & TYPEINFO_UNTYPED_INTEGER); }
	bool isUntypedFloat() const { return (flags & TYPEINFO_UNTYPED_FLOAT); }
	bool isUntypedBinHex() const { return (flags & TYPEINFO_UNTYPED_BIN_HEX); }
	bool isConstAlias() const { return (flags & TYPEINFO_CONST_ALIAS); }
	bool isCharacter() const { return (flags & TYPEINFO_CHARACTER); }

	virtual bool      isSame(const TypeInfo* from, CastFlags castFlags) const;
	virtual TypeInfo* clone() = 0;
	virtual uint32_t  numRegisters() const;
	virtual Utf8      getDisplayName();
	virtual void      computeWhateverName(Utf8& resName, uint32_t nameType);

	const char* getDisplayNameC();
	void        copyFrom(const TypeInfo* from);
	void        setConst();

	void computeName() { computeWhateverName(COMPUTE_NAME); }
	void computeScopedName() { computeWhateverName(COMPUTE_SCOPED_NAME); }
	void computeScopedNameExport() { computeWhateverName(COMPUTE_SCOPED_NAME_EXPORT); }

	void        removeGenericFlag();
	void        clearName();
	void        forceComputeName();
	void        computeScopedName(Utf8& newName) const;
	Utf8        getName();
	Utf8        getTypeName(bool forceNoScope);
	const Utf8& computeWhateverName(uint32_t nameType);
	const Utf8& computeWhateverNameNoLock(uint32_t nameType);

	mutable SharedMutex mutex;

	Utf8 name;
	Utf8 displayName;
	Utf8 scopedName;
	Utf8 scopedNameExport;

	AstNode* declNode = nullptr;
	uint64_t flags    = 0;

	uint32_t       sizeOf     = 0;
	TypeInfoKind   kind       = TypeInfoKind::Invalid;
	NativeTypeKind nativeType = NativeTypeKind::Void;
	uint8_t        padding[2];

	SWAG_RACE_CONDITION_INSTANCE(raceName);
};

struct TypeInfoParam
{
	uint32_t       numRegisters() const;
	bool           isSame(const TypeInfoParam* to, CastFlags castFlags) const;
	TypeInfoParam* clone() const;
	void           allocateComputedValue();

	Utf8          name;
	AttributeList attributes;

	ComputedValue* value    = nullptr;
	TypeInfo*      typeInfo = nullptr;
	AstNode*       declNode = nullptr;

	uint32_t flags : 8  = 0;
	uint32_t index : 24 = 0;
	uint32_t offset     = 0;
};

struct TypeInfoNative final : TypeInfo
{
	TypeInfoNative()
		: TypeInfo{TypeInfoKind::Native}
	{
		valueInteger = 0;
	}

	TypeInfoNative(NativeTypeKind type, const char* tname, int sof, uint64_t fl)
	{
		kind       = TypeInfoKind::Native;
		nativeType = type;
		name.setView(tname, static_cast<int>(strlen(tname)));
		sizeOf       = sof;
		flags        = fl;
		valueInteger = 0;
	}

	bool      isSame(const TypeInfo* to, CastFlags castFlags) const override;
	TypeInfo* clone() override;

	union
	{
		int32_t valueInteger;
		float   valueFloat;
	};
};

struct TypeInfoNamespace final : TypeInfo
{
	TypeInfoNamespace()
		: TypeInfo{TypeInfoKind::Namespace}
	{
	}

	TypeInfo* clone() override;

	Scope* scope = nullptr;
};

struct TypeInfoEnum final : TypeInfo
{
	TypeInfoEnum()
		: TypeInfo{TypeInfoKind::Enum}
	{
	}

	bool      isSame(const TypeInfo* to, CastFlags castFlags) const override;
	Utf8      getDisplayName() override;
	TypeInfo* clone() override;

	bool contains(const Utf8& valueName);
	void collectEnums(VectorNative<TypeInfoEnum*>& collect);

	VectorNative<TypeInfoParam*> values;
	AttributeList                attributes;

	Scope*    scope   = nullptr;
	TypeInfo* rawType = nullptr;
};

struct TypeInfoFuncAttr final : TypeInfo
{
	TypeInfoFuncAttr()
		: TypeInfo{TypeInfoKind::FuncAttr}
	{
	}

	uint32_t numRegisters() const override
	{
		return 1;
	}

	TypeInfo* clone() override;
	void      computeWhateverName(Utf8& resName, uint32_t nameType) override;
	bool      isSame(const TypeInfo* to, CastFlags castFlags) const override;

	bool            isSame(const TypeInfoFuncAttr* other, CastFlags castFlags, BadSignatureInfos& bi) const;
	bool            isSame(const TypeInfoFuncAttr* other, CastFlags castFlags) const;
	TypeInfo*       concreteReturnType() const;
	bool            isFctVariadic() const;
	bool            isFctCVariadic() const;
	uint32_t        registerIdxToParamIdx(uint32_t argIdx);
	TypeInfo*       registerIdxToType(int argIdx);
	uint32_t        numParamsRegisters();
	uint32_t        numReturnRegisters() const;
	uint32_t        numTotalRegisters();
	const CallConv& getCallConv() const;

	VectorNative<TypeInfoParam*>        capture;
	VectorNative<TypeInfoParam*>        genericParameters;
	VectorNative<TypeInfoParam*>        parameters;
	AttributeList                       attributes;
	VectorMap<Utf8, GenericReplaceType> replaceTypes;
	VectorMap<Utf8, ComputedValue*>     replaceValues;

	TypeInfo* returnType = nullptr;

	uint32_t     firstDefaultValueIdx = UINT32_MAX;
	uint32_t     stackSize            = 0;
	uint32_t     attributeUsage       = All;
	CallConvKind callConv             = Swag;
};

struct TypeInfoPointer final : TypeInfo
{
	TypeInfoPointer()
		: TypeInfo{TypeInfoKind::Pointer}
	{
	}

	void      computeWhateverName(Utf8& resName, uint32_t nameType) override;
	Utf8      getDisplayName() override;
	bool      isSame(const TypeInfo* to, CastFlags castFlags) const override;
	TypeInfo* clone() override;

	TypeInfo* pointedType = nullptr;
};

struct TypeInfoArray final : TypeInfo
{
	TypeInfoArray()
		: TypeInfo{TypeInfoKind::Array}
	{
	}

	uint32_t numRegisters() const override
	{
		return 1;
	}

	void      computeWhateverName(Utf8& resName, uint32_t nameType) override;
	bool      isSame(const TypeInfo* to, CastFlags castFlags) const override;
	TypeInfo* clone() override;

	TypeInfo* pointedType = nullptr;
	TypeInfo* finalType   = nullptr;
	AstNode*  sizeNode    = nullptr;

	uint32_t count      = 0;
	uint32_t totalCount = 0;
};

struct TypeInfoSlice final : TypeInfo
{
	TypeInfoSlice()
		: TypeInfo{TypeInfoKind::Slice}
	{
		sizeOf = 2 * sizeof(void*);
	}

	void      computeWhateverName(Utf8& resName, uint32_t nameType) override;
	bool      isSame(const TypeInfo* to, CastFlags castFlags) const override;
	TypeInfo* clone() override;

	TypeInfo* pointedType = nullptr;
};

struct TypeInfoList final : TypeInfo
{
	TypeInfoList() = default;

	uint32_t numRegisters() const override
	{
		return 1;
	}

	bool      isSame(const TypeInfo* to, CastFlags castFlags) const override;
	TypeInfo* clone() override;
	void      computeWhateverName(Utf8& resName, uint32_t nameType) override;

	VectorNative<TypeInfoParam*> subTypes;

	Scope* scope = nullptr;
};

struct TypeInfoVariadic final : TypeInfo
{
	TypeInfoVariadic()
		: TypeInfo{TypeInfoKind::Variadic}
	{
	}

	bool      isSame(const TypeInfo* to, CastFlags castFlags) const override;
	TypeInfo* clone() override;
	void      computeWhateverName(Utf8& resName, uint32_t nameType) override;

	TypeInfo* rawType = nullptr;
};

struct TypeInfoGeneric final : TypeInfo
{
	TypeInfoGeneric()
		: TypeInfo{TypeInfoKind::Generic}
	{
		flags |= TYPEINFO_GENERIC;
	}

	bool      isSame(const TypeInfo* to, CastFlags castFlags) const override;
	TypeInfo* clone() override;

	TypeInfo* rawType = nullptr;
};

struct TypeInfoStruct final : TypeInfo
{
	TypeInfoStruct()
		: TypeInfo{TypeInfoKind::Struct}
	{
	}

	uint32_t numRegisters() const override
	{
		if (kind == TypeInfoKind::Interface)
			return 2;
		return 1;
	}

	bool      isSame(const TypeInfo* to, CastFlags castFlags) const override;
	TypeInfo* clone() override;
	Utf8      getDisplayName() override;
	void      computeWhateverName(Utf8& resName, uint32_t nameType) override;

	TypeInfoParam* findChildByNameNoLock(const Utf8& childName) const;
	TypeInfoParam* hasInterface(const TypeInfoStruct* itf) const;
	TypeInfoParam* hasInterfaceNoLock(const TypeInfoStruct* itf) const;
	static Utf8    computeTupleDisplayName(const VectorNative<TypeInfoParam*>& fields, uint32_t nameType);
	bool           canRawCopy() const;
	bool           isPlainOldData() const;
	void           flattenUsingFields();

	VectorNative<TypeInfoParam*>          genericParameters;
	VectorNative<TypeInfo*>               deducedGenericParameters;
	VectorNative<TypeInfoParam*>          fields;
	VectorNative<TypeInfoParam*>          consts;
	VectorNative<TypeInfoParam*>          methods;
	VectorNative<TypeInfoParam*>          interfaces;
	VectorNative<TypeInfoParam*>          flattenFields;
	VectorMap<Utf8, GenericReplaceType>   replaceTypes;
	VectorMap<Utf8, ComputedValue*>       replaceValues;
	VectorMap<TypeInfo*, SymbolOverload*> mapOpCast;
	VectorMap<TypeInfo*, SymbolOverload*> mapOpAffect[4];
	AttributeList                         attributes;
	Utf8                                  structName;
	SharedMutex                           mutexCache;
	SharedMutex                           mutexGen;

	TypeInfoStruct* itable            = nullptr;
	TypeInfoStruct* fromGeneric       = nullptr;
	Scope*          scope             = nullptr;
	ByteCode*       opInit            = nullptr;
	AstFuncDecl*    opUserInitFct     = nullptr;
	ByteCode*       opDrop            = nullptr;
	AstFuncDecl*    opUserDropFct     = nullptr;
	ByteCode*       opPostCopy        = nullptr;
	AstFuncDecl*    opUserPostCopyFct = nullptr;
	ByteCode*       opPostMove        = nullptr;
	AstFuncDecl*    opUserPostMoveFct = nullptr;

	uint32_t alignOf                    = 0;
	uint32_t cptRemainingInterfaces     = 0;
	uint32_t cptRemainingInterfacesReg  = 0;
	uint32_t cptRemainingMethods        = 0;
	uint32_t cptRemainingSpecialMethods = 0;

	SWAG_RACE_CONDITION_INSTANCE(raceFields);
};

struct TypeInfoAlias final : TypeInfo
{
	TypeInfoAlias()
		: TypeInfo{TypeInfoKind::Alias}
	{
	}

	void      computeWhateverName(Utf8& resName, uint32_t nameType) override;
	bool      isSame(const TypeInfo* to, CastFlags castFlags) const override;
	TypeInfo* clone() override;

	uint32_t numRegisters() const override
	{
		return rawType->numRegisters();
	}

	TypeInfo* rawType = nullptr;
};

struct TypeInfoCode final : TypeInfo
{
	TypeInfoCode()
		: TypeInfo{"code", TypeInfoKind::Code}
	{
	}

	bool      isSame(const TypeInfo* to, CastFlags castFlags) const override;
	TypeInfo* clone() override;

	AstNode* content = nullptr;
};

template<typename T>
T* castTypeInfo(TypeInfo* ptr, TypeInfoKind kind)
{
	SWAG_ASSERT(ptr);
	T* casted = static_cast<T*>(ptr->getConstAlias());
	SWAG_ASSERT(casted->kind == kind);
	return casted;
}

template<typename T>
T* castTypeInfo(TypeInfo* ptr, TypeInfoKind kind1, TypeInfoKind kind2)
{
	SWAG_ASSERT(ptr);
	T* casted = static_cast<T*>(ptr->getConstAlias());
	SWAG_ASSERT(casted->kind == kind1 || casted->kind == kind2);
	return casted;
}

template<typename T>
const T* castTypeInfo(const TypeInfo* ptr, TypeInfoKind kind)
{
	SWAG_ASSERT(ptr);
	const T* casted = static_cast<const T*>(ptr->getConstAlias());
	SWAG_ASSERT(casted->kind == kind);
	return casted;
}

template<typename T>
const T* castTypeInfo(const TypeInfo* ptr, TypeInfoKind kind1, TypeInfoKind kind2)
{
	SWAG_ASSERT(ptr);
	const T* casted = static_cast<const T*>(ptr->getConstAlias());
	SWAG_ASSERT(casted->kind == kind1 || casted->kind == kind2);
	return casted;
}

template<typename T>
T* castTypeInfo(TypeInfo* ptr)
{
	return static_cast<T*>(ptr);
}

template<typename T>
const T* castTypeInfo(const TypeInfo* ptr)
{
	return static_cast<const T*>(ptr);
}
