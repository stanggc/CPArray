#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "CPArray.h"

// asprintf and friends, for Windows.
#if defined(WINDOWS)
int vasprintf(char **out, const char *fmt, va_list args) {
	int expectedSize = 0;
	char *content = NULL;
	size_t bufSize = 0;
	va_list argsCopy;

	va_copy(argsCopy, args);
	expectedSize = vsnprintf(NULL, 0, fmt, argsCopy);
	va_end(argsCopy);

	bufSize = expectedSize + 1;

	content = calloc(bufSize, sizeof(char));
	if (!content) {
		if (out) *out = NULL;
		expectedSize = -1;
		goto cleanUp;
	}
	vsnprintf(content, bufSize, fmt, args);

	if (out) *out = content;

cleanUp:
	return expectedSize;
}

int asprintf(char **out, const char *fmt, ...) {
	va_list args, argsCopy;
	size_t expectedSize = 0;
	char *content = NULL;
	size_t bufSize = 0;

	va_start(args, fmt);

	va_copy(argsCopy, args);
	expectedSize = vsnprintf(NULL, 0, fmt, argsCopy);
	va_end(argsCopy);

	bufSize = expectedSize + 1;

	content = calloc(bufSize, sizeof(char));
	if (!content) {
		if (out) *out = NULL;
		expectedSize = -1;
		goto cleanUp;
	}

	expectedSize = vsnprintf(content, bufSize, fmt, args);
	if (out) *out = content;

cleanUp:
	va_end(args);
	return expectedSize;
}
#endif


thread_local enum CPAStatusCode CPAStatusCode;

struct CPArray {
	size_t Length;
	size_t Capacity;
	char **Array;
};

static bool EnsureObjectProvidedOrFlagError(CPArray *arr) {
	if (!arr) {
		CPAStatusCode = CPA_ObjectRequired;
		return false;
	}
	return true;
}

static void CPArray_FreeContent(char *content) {
	if (content) free(content);
}

CPArray * CPArray_Init(size_t capacity) {
	CPArray *arr = calloc(1, sizeof(CPArray));

	if (!arr) {
		CPAStatusCode = CPA_Mem;
		return NULL;
	}

	arr->Array = (char **) calloc(capacity, sizeof(char *));
	if (!arr->Array) {
		CPAStatusCode = CPA_Mem;
		free(arr);
		return NULL;
	}

	arr->Capacity = capacity;

	return arr;
}

int CPArray_Deinit(CPArray *arr) {
	if (!EnsureObjectProvidedOrFlagError(arr)) return -1;

	for (off64_t i = 0; i < arr->Length; ++i) {
		CPArray_FreeContent(arr->Array[i]);
	}
	if (arr && arr->Array) free(arr->Array);
	if (arr) free(arr);

	return 0;
}

size_t CPArray_Capacity(CPArray *arr) {
	if (!EnsureObjectProvidedOrFlagError(arr)) return -1;

	return arr->Capacity;
}

size_t CPArray_Length(CPArray *arr) {
	if (!EnsureObjectProvidedOrFlagError(arr)) return -1;

	return arr->Length;
}

int CPArray_Insert(CPArray *arr, off64_t index, const char *content) {
	char *contentCopy = NULL;

	if (!EnsureObjectProvidedOrFlagError(arr)) return -1;

	if (index >= arr->Capacity || index < 0) {
		CPAStatusCode = CPA_IndexPastCapacity;
		return -1;
	}
	if (arr->Length + 1 > arr->Capacity) {
		CPAStatusCode = CPA_CapacityReached;
		return -1;
	}

	// Starting from the end of the array, shift elements to the right
	// until we are at the specified index.
	for (off64_t i = arr->Length; i >= 1; --i) {
		// Make sure that we do not write past our memory bounds.
		if (i >= arr->Capacity) {
			CPAStatusCode = CPA_IndexOutOfBounds;
			return -1;
		}
		if (i == index) break;
		arr->Array[i] = arr->Array[i - 1];
	}
	if (asprintf(&contentCopy, "%s", content) == -1) {
		CPAStatusCode = CPA_Mem;
		return -1;
	}
	arr->Array[index] = contentCopy;
	++arr->Length;

	return 0;
}

int CPArray_Remove(CPArray *arr, off64_t index) {
	if (!EnsureObjectProvidedOrFlagError(arr)) return -1;

	if (index >= arr->Length || index < 0 || (arr->Length == 0 && index == 0)) {
		CPAStatusCode = CPA_IndexOutOfBounds;
		return -1;
	}

	// Shift elements left, starting from the specified index.
	for (off64_t i = index; i < arr->Length; ++i) {
		arr->Array[i] = i + 1 >= arr->Length ? NULL : arr->Array[i + 1];
	}
	--arr->Length;

	return 0;
}

char * CPArray_At(CPArray *arr, off64_t index) {
	if (!EnsureObjectProvidedOrFlagError(arr)) return NULL;

	if (index >= arr->Length || index < 0) {
		CPAStatusCode = CPA_IndexOutOfBounds;
		return NULL;
	}

	return arr->Array[index];
}

const char * CPArray_StatusCodeMessage(enum CPAStatusCode code) {
	switch (code) {
	case CPA_OK:
		return "OK";
	case CPA_Mem:
		return "memory error";
	case CPA_ObjectRequired:
		return "object required";
	case CPA_IndexOutOfBounds:
		return "index out of bounds";
	case CPA_IndexPastCapacity:
		return "index past capacity";
	case CPA_CapacityReached:
		return "capacity reached";
	default:
		return "unknown status code";
	}
}
