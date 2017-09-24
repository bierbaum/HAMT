/**
 *      File: main.cpp
 *    Author: CS Lim
 *   Purpose: Simple HAMT test program
 *   History:
 *  2012/5/3: File Created
 *
 */

#include <config.h>     // autogenerated by CMake
#include <cstdio>
#include <cstdint>
#include <cassert>
#include <conio.h>

// Use dlmalloc so that benchmark testing is less affected
// by platform specific heap manager implementation.
// e.g. Windows LFH
#if HAMT_TEST_USE_DLMALLOC

extern "C"
{
    void* dlmalloc(size_t);
    void* dlcalloc(size_t, size_t);
    void  dlfree(void*);
    void* dlrealloc(void*, size_t);
}

#define malloc dlmalloc
#define calloc dlcalloc
#define free dlfree
#define realloc dlrealloc

inline void* operator new(size_t cb)
{
    return dlmalloc(cb);
}

inline void* operator new[](size_t cb)
{
    return dlmalloc(cb);
}

inline void operator delete(void* p)
{
    dlfree(p);
}

inline void operator delete[](void* p)
{
    dlfree(p);
}

#endif

#include <HashTrie.h>


typedef unsigned char u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef signed char s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

const uint32 MAX_TEST_ENTRIES = 1000000;

#ifdef WIN32

#include <conio.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

//===========================================================================
// Timing functions
//===========================================================================
u64 GetMicroTime()
{
    u64 hz;
    QueryPerformanceFrequency((LARGE_INTEGER*)&hz);

    u64 t;
    QueryPerformanceCounter((LARGE_INTEGER*)&t);
    return (t * 1000000) / hz;
}

#else

#include <sys/time.h>
u64 GetMicroTime()
{
    timeval t;
    gettimeofday(&t,NULL);
    return t.tv_sec * 1000000ull + t.tv_usec;
}

#endif

void TestHashTrie ()
{
    // uint32 to uint32 key/vaue pair example
    struct Test : THashKey32<uint32>
    {
        Test(uint32 key) : THashKey32<uint32>(key) { }
        uint32 value{ 0 };
    };

    // string to uint32 key/vaue pair example
    struct TestStr : CHashKeyStrAnsiChar
    {
        TestStr(const char key[]) : CHashKeyStrAnsiChar(key) { }
        uint32 value{ 0 };
    };

    //
    // int test
    //
    HASH_TRIE(Test, THashKey32<uint32>) test_uint32;

    printf("32 bit integer test...\n");
    printf("1) Add %d entries:    ", MAX_TEST_ENTRIES);
    u64 t0 = GetMicroTime();
    for (int i = 0; i < MAX_TEST_ENTRIES; i++)
    {
        auto test = new Test(i);
        test_uint32.Add(test);
    }
    printf("   %10u usec\n", int(GetMicroTime() - t0));

    printf("2) Find %d entries:   ", MAX_TEST_ENTRIES);
    t0 = GetMicroTime();
    for (uint32 i = 0; i < MAX_TEST_ENTRIES; i++) {
        auto find = test_uint32.Find(THashKey32<uint32>(i));
        volatile uint32 value = find->Get();
        assert(value == i);
    }
    printf("   %10u usec\n", int(GetMicroTime() - t0));

    printf("3) Remove %d entries: ", MAX_TEST_ENTRIES);
    t0 = GetMicroTime();
    for (uint32 i = 0; i < MAX_TEST_ENTRIES; i++)
    {
        Test *removed = test_uint32.Remove(THashKey32<uint32>(i));
        assert(removed != 0);
        assert(removed->Get() == i);
        delete removed;
    }
    printf("   %10u usec\n\n", int(GetMicroTime() - t0));

    // THashTrieInt test
    THashTrieInt<int32> test_hashTrieInt;

    printf("32 bit integer test using THashTrieInt...\n");
    printf("1) Add %d entries:    ", MAX_TEST_ENTRIES);
    t0 = GetMicroTime();
    for (int32 i = 0; i < MAX_TEST_ENTRIES; i++)
    {
        auto added = test_hashTrieInt.Add(i);
        added->value = i;
    }
    printf("   %10u usec\n", int(GetMicroTime() - t0));

    printf("2) Find %d entries:   ", MAX_TEST_ENTRIES);
    t0 = GetMicroTime();
    for (int32 i = 0; i < MAX_TEST_ENTRIES; i++)
    {
        volatile auto * find = test_hashTrieInt.Find(i);
        assert(find->value == i);
    }
    printf("   %10u usec\n", int(GetMicroTime() - t0));

    printf("3) Remove %d entries: ", MAX_TEST_ENTRIES);
    t0 = GetMicroTime();
    for (int32 i = 0; i < MAX_TEST_ENTRIES; i++)
    {
        bool removed = test_hashTrieInt.Remove(i);
        assert(removed);
    }
    printf("   %10u usec\n\n", int(GetMicroTime() - t0));

    //
    // String hash test
    //
    HASH_TRIE(TestStr, CHashKeyStrAnsiChar)    test_str;

    printf("ANSI string test...\n");
    printf("1) Add %d entries:    ", MAX_TEST_ENTRIES);
    t0 = GetMicroTime();
    for (uint32 i = 0; i < MAX_TEST_ENTRIES; i++)
    {
        char buffer[16];
        sprintf_s(buffer, "%d", i);
        TestStr *test = new TestStr(buffer);
        test_str.Add(test);
    }
    printf("   %10u usec\n", int(GetMicroTime() - t0));

    printf("2) Find %d entries:   ", MAX_TEST_ENTRIES);
    t0 = GetMicroTime();
    for (uint32 i = 0; i < MAX_TEST_ENTRIES; i++)
    {
        char buffer[16];
        sprintf_s(buffer, "%d", i);
        TestStr *find = test_str.Find(CHashKeyStrAnsiChar(buffer));
        assert(strcmp(find->GetString(), buffer) == 0);
    }
    printf("   %10u usec\n", int(GetMicroTime() - t0));

    printf("3) Remove %d entries: ", MAX_TEST_ENTRIES);
    t0 = GetMicroTime();
    for (uint32 i = 0; i < MAX_TEST_ENTRIES; i++)
    {
        char buffer[16];
        sprintf_s(buffer, "%d", i);
        TestStr *removed2 = test_str.Remove(CHashKeyStrAnsiChar(buffer));
        assert(removed2 != 0);
        assert(strcmp(removed2->GetString(), buffer) == 0);
        delete removed2;
    }
    printf("   %10u usec\n\n", int(GetMicroTime() - t0));
}

int main()
{
    TestHashTrie();
    return 0;
}
