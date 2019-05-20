#pragma once
class Module
{
public:
    fs::path m_path;
    string   m_name;

public:
    Module(const fs::path& path)
        : m_path{path}
    {
		m_name = path.filename().string();
    }
};
