#pragma once

class CModule {
   public:
    CModule(const char* name);
    ~CModule();

    auto GetHandle() const { return m_Handle; }
    auto GetName() const { return m_Name.c_str(); }
    auto GetBaseAddress() const { return m_Begin; }
    auto GetSize() const { return m_Size; }

    // Returns false if module not loaded.
    bool Retrieve();

    uintptr_t GetInterface(uint32_t versionHash);
    uintptr_t GetProcAddress(const char* procName);
    uintptr_t FindPattern(const std::span<const int>& pattern) const;

   private:
    void InitializeHandle();
    void InitializeBounds();

    void SetBounds(uintptr_t begin, uintptr_t size);

    // Module name.
    std::string m_Name;

    // Module handle.
    void* m_Handle;

    // Used for pattern scanning.
    uintptr_t m_Begin, m_Size;

    std::unordered_map<std::uint32_t, std::uintptr_t> _interfaces {};
};
