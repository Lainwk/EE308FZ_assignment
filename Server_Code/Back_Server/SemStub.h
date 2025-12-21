#pragma once

// Windows stub implementations; original SysV SemStubaphore logic is no longer used
// now that inter-process communication was replaced by TCP sockets.
inline int SemStub_creat(int, int) { return 0; }
inline int SemStub_setValue(int, int, int) { return 0; }
inline int SemStub_p(int, int) { return 0; }
inline int SemStub_v(int, int) { return 0; }
