#pragma once

// For thread-local storage.
#define thread_local __thread

#include <sys/types.h>

#if defined(__WIN32) && !defined(WINDOWS)
#define WINDOWS
#endif

#if defined(MACOS)
#define off64_t off_t
#define fopen64 fopen
#define fseeko64 fseeko
#define ftello64 ftello
#define stat64 stat
#endif

enum CPAStatusCode {
	CPA_OK = 0,
	CPA_Mem,
	CPA_ObjectRequired,
	CPA_IndexOutOfBounds,
	CPA_IndexPastCapacity,
	CPA_CapacityReached,
};

extern thread_local enum CPAStatusCode CPAStatusCode;

struct CPArray;
typedef struct CPArray CPArray;

CPArray * CPArray_Init(size_t capacity);
int CPArray_Deinit(CPArray *arr);
size_t CPArray_Capacity(CPArray *arr);
size_t CPArray_Length(CPArray *arr);
int CPArray_Insert(CPArray *arr, off64_t index, const char *content);
int CPArray_Remove(CPArray *arr, off64_t index);
char * CPArray_At(CPArray *arr, off64_t index);
const char * CPArray_StatusCodeMessage(enum CPAStatusCode code);
