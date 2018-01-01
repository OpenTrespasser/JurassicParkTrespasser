// Metrowerks Build uses MSL, which defines the
// items previously in memory.h (that is, the
// items in the HP STL memory.h) elsewhere.

// The function declarations are taken from <cstring>

// 98.08.28, ARS: Created
// 98.09.10, ARS: Moved into global namespace; not part of
//                STL, these are part of the C runtime.
// 98.09.11, ARS: Moved back into std::, but with the extern qualifier

namespace std {
  extern "C" {
	void * 	memset(void * dst, int val, size_t len);
	void * memcpy (void * dst, const void * src, size_t len);
	int memcmp(const void * src1, const void * src2, size_t len);
  }
}

using std::memcpy;
using std::memcmp;
using std::memset;
