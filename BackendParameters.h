#pragma once
enum class BackendType
{
    Exe,
    Dll,
    Lib,
};

struct BackendParameters
{
    BackendType type = BackendType::Exe;
};
