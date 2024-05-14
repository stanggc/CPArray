#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "CPArray.h"


#if defined(MACOS) || defined(LINUX) || defined(UNIX)
#include <locale.h>
static void SetConsoleCodePage(const char *cpName) {
	if (!strcmp(cpName, "utf-8")) {
		setlocale(LC_ALL, "C.UTF-8");
	}
}
#elif defined(WINDOWS) || defined(_WIN32)
#include <windows.h>
static void SetConsoleCodePage(const char *cpName) {
	if (!strcmp(cpName, "utf-8")) {
		SetConsoleOutputCP(65001);
	}
}
#endif

static const char *unknownStatusCode = "unknown status code";


static void Test_InsertBigScale() {
	CPArray *arr = NULL;
	size_t lSize = 0;
	char *contentRetrieved = NULL;
	char *fmt = "%lld";

	// On Linux, off64_t is not a `long long int`.
	// Other UNIX OSes, if using GNU libc, likely
	// to be similar.
#if defined(LINUX) || defined(UNIX)
	fmt = "%ld";
#endif

	// Test big-scale add.
	assert((arr = CPArray_Init(2100000)));
	for (off64_t i = 0; i < 2000000; ++i) {
		char s[1024] = { 0 };
		sprintf(s, fmt, i);
		lSize = CPArray_Length(arr);
		assert(!CPAStatusCode);
		assert(!CPArray_Insert(arr, lSize, s));
	}
	assert((contentRetrieved = CPArray_At(arr, 80000)));
	assert(!strcmp(contentRetrieved, "80000"));

	// Test remove.
	assert(!CPArray_Remove(arr, 80000));
	contentRetrieved = NULL;
	contentRetrieved = CPArray_At(arr, 80000);
	assert(!strcmp(contentRetrieved, "80001"));
	assert(!CPArray_Deinit(arr));
}

int main(int argc, char *argv[]) {
	char content[256] = { 0 };
	char content2[256] = { 0 };
	char content3[256] = { 0 };
	char *contentRetrieved = NULL;
	CPArray *arr = NULL;
	off64_t index = 0;
	size_t lSize = 0;

	SetConsoleCodePage("utf-8");

	sprintf(content, "hello.");
	sprintf(content2, "there.");
	sprintf(content3, "OK!");

	// Test init and deinit.
	assert((arr = CPArray_Init(1000)));
	assert(!CPArray_Deinit(arr));


	// Test insertion until capacity is reached.
	arr = NULL;
	assert((arr = CPArray_Init(2)));
	assert(!CPArray_Insert(arr, 0, content));
	assert(!CPArray_Insert(arr, 0, content2));
	assert((contentRetrieved = CPArray_At(arr, 0)));
	assert(!strcmp(contentRetrieved, content2));
	assert((contentRetrieved = CPArray_At(arr, 1)));
	assert(!strcmp(contentRetrieved, content));
	assert(!CPArray_Deinit(arr));


	// Re-init for the tests that follows.
	arr = NULL;
	assert((arr = CPArray_Init(1000)));

	// Test add.
	// Insert first element.
	assert(!CPArray_Insert(arr, 0, content));
	assert((contentRetrieved = CPArray_At(arr, 0)));
	assert(!strcmp(content, contentRetrieved));

	// Insert another element at the front.
	assert(!CPArray_Insert(arr, 0, content2));
	contentRetrieved = CPArray_At(arr, 0);
	assert(!strcmp(content2, contentRetrieved));
	contentRetrieved = CPArray_At(arr, 1);
	assert(!strcmp(content, contentRetrieved));
	assert((lSize = CPArray_Length(arr)) == 2);

	// Insert another element at the end.
	assert(!CPArray_Insert(arr, lSize, content3));
	index = lSize;
	assert((lSize = CPArray_Length(arr)) == 3);
	assert((contentRetrieved = CPArray_At(arr, index)));
	assert(!strcmp(content3, contentRetrieved));
	contentRetrieved = NULL;
	contentRetrieved = CPArray_At(arr, 2);
	assert(!strcmp(content3, contentRetrieved));

	// Test retrieval of non-existent element.
	assert(!CPArray_At(arr, 3) && CPAStatusCode == CPA_IndexOutOfBounds);
	CPAStatusCode = CPA_OK;

	// Test remove.
	assert(!CPArray_Remove(arr, 1));
	assert((lSize = CPArray_Length(arr)) == 2);
	contentRetrieved = NULL;
	contentRetrieved = CPArray_At(arr, 1);
	assert(!strcmp(contentRetrieved, content3));
	// Test remove the remaining.
	assert(!CPArray_Remove(arr, 0));
	contentRetrieved = NULL;
	assert((contentRetrieved = CPArray_At(arr, 0)));
	assert(!strcmp(contentRetrieved, content3));
	assert(CPArray_Remove(arr, 1) && CPAStatusCode == CPA_IndexOutOfBounds);
	CPAStatusCode = CPA_OK;
	assert(CPArray_Remove(arr, -1) && CPAStatusCode == CPA_IndexOutOfBounds);
	CPAStatusCode = CPA_OK;
	assert(!CPArray_Remove(arr, 0));
	assert((lSize = CPArray_Length(arr)) == 0 && !CPAStatusCode);
	// By this time, the arr is empty. We test removal.
	assert(CPArray_Remove(arr, 0) && CPAStatusCode == CPA_IndexOutOfBounds);
	CPAStatusCode = CPA_OK;
	assert((lSize = CPArray_Length(arr)) == 0 && !CPAStatusCode);
	assert(!CPArray_Deinit(arr));

	if (argc >= 2 && !strcmp(argv[1], "all")) {
		printf("Test big-scale.\n");
		Test_InsertBigScale();
	}

	// Test status code messages.
	assert(strcmp(CPArray_StatusCodeMessage(CPA_OK), unknownStatusCode));
	assert(strcmp(CPArray_StatusCodeMessage(CPA_Mem), unknownStatusCode));
	assert(strcmp(CPArray_StatusCodeMessage(CPA_ObjectRequired), unknownStatusCode));
	assert(strcmp(CPArray_StatusCodeMessage(CPA_IndexOutOfBounds), unknownStatusCode));
	assert(strcmp(CPArray_StatusCodeMessage(CPA_IndexPastCapacity), unknownStatusCode));
	assert(strcmp(CPArray_StatusCodeMessage(CPA_CapacityReached), unknownStatusCode));

	printf("OK了: Test_CPArray\n");

	return 0;
}
