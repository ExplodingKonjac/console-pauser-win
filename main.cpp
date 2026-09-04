// clang-format off
#include <windows.h>
#include <psapi.h>
// clang-format on

#include <cstdio>
#include <cwctype>

static double sec(FILETIME t) {
    ULARGE_INTEGER x{t.dwLowDateTime, t.dwHighDateTime};
    return x.QuadPart / 1e7;
}

static void pause() {
    HANDLE h = CreateFileW(
        L"CONIN$", GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr
    );
    if (h == INVALID_HANDLE_VALUE) return;

    std::fputs("Press any key to continue...", stderr);
    std::fflush(stderr);

    FlushConsoleInputBuffer(h);  // 丢掉子程序运行期间积压的输入

    INPUT_RECORD r;
    DWORD n;
    do {
        ReadConsoleInputW(h, &r, 1, &n);
    } while (r.EventType != KEY_EVENT || !r.Event.KeyEvent.bKeyDown);
    CloseHandle(h);
}

int main(int argc, char**) {
    if (argc < 2) {
        std::fputs("Usage: console-pauser <prog> <args>...\n", stderr);
        return 2;
    }

    wchar_t* cmd = GetCommandLineW();
    if (*cmd == L'"') {
        for (++cmd; *cmd && *cmd != L'"'; ++cmd);
        if (*cmd) ++cmd;
    } else {
        while (*cmd && !iswspace(*cmd)) ++cmd;
    }
    while (iswspace(*cmd)) ++cmd;

    STARTUPINFOW si{};
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi{};

    if (!CreateProcessW(
            nullptr, cmd, nullptr, nullptr, TRUE, 0, nullptr, nullptr, &si, &pi
        )) {
        std::fprintf(stderr, "CreateProcess failed: %lu\n", GetLastError());
        pause();
        return 1;
    }

    CloseHandle(pi.hThread);
    WaitForSingleObject(pi.hProcess, INFINITE);

    FILETIME a, b, sys, user;
    PROCESS_MEMORY_COUNTERS pmc{};
    pmc.cb = sizeof(pmc);
    GetProcessTimes(pi.hProcess, &a, &b, &sys, &user);
    GetProcessMemoryInfo(pi.hProcess, &pmc, sizeof(pmc));

    DWORD code;
    GetExitCodeProcess(pi.hProcess, &code);

    std::fprintf(
        stderr,
        "\nUser time:   %.3f s\n"
        "System time: %.3f s\n"
        "Memory:      %.2f MiB\n\n",
        sec(user), sec(sys), pmc.PeakWorkingSetSize / 1048576.0
    );

    CloseHandle(pi.hProcess);
    pause();
    return code;
}