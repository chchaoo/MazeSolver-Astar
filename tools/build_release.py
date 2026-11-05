"""Build a portable Windows executable with the original runtime assets embedded."""
import hashlib
from pathlib import Path
import subprocess
import urllib.request

ROOT = Path(__file__).resolve().parents[1]
BUILD = ROOT / 'build-release'
DIST = ROOT / 'dist'
EASYX_URL = 'https://easyx.cn/download/EasyX_26.10.25.exe'
EASYX_SHA256 = '782923f4ab3abcda4ad6e54563d3c9de8a83955db04481520e6ad3a48eddc3b7'


def main():
    BUILD.mkdir(exist_ok=True)
    DIST.mkdir(exist_ok=True)
    installer = BUILD / 'easyx.7z'
    with urllib.request.urlopen(EASYX_URL, timeout=90) as response:
        installer.write_bytes(response.read())
    assert hashlib.sha256(installer.read_bytes()).hexdigest() == EASYX_SHA256
    library = BUILD / 'easyx'
    subprocess.run(['7z', 'x', str(installer), '-o' + str(library), '-y'], check=True)
    header = next(library.rglob('graphics.h'))
    candidates = [p for p in library.rglob('EasyXw.lib') if 'x64' in p.parts]
    candidates.sort(key=lambda p: ('VC2015' not in p.parts, str(p)))
    if not candidates:
        raise RuntimeError('The official package does not contain the x64 library')
    assets = sorted((ROOT / 'test').glob('*.jpg')) + [ROOT / 'test/music.wav', ROOT / 'LICENSE', ROOT / 'NOTICE.md']
    names = [p.name for p in assets]
    assert len(names) == len(set(names))
    resources, entries = [], []
    for index, path in enumerate(assets, 101):
        resources.append(f'{index} RCDATA "{str(path).replace(chr(92), chr(92) * 2)}"')
        entries.append(f'{{{index}, L"{path.name}", {path.stat().st_size}}}')
    (BUILD / 'assets.rc').write_text('\n'.join(resources), encoding='utf-8')
    wrapper = r'''#include <windows.h>
#include <mmsystem.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#define main OriginalMazeMain
#include "../test/test.cpp"
#undef main
#pragma pack()
struct EmbeddedFile { int id; const wchar_t* name; size_t size; };
static const EmbeddedFile files[] = { ENTRIES };
int main(int argc, char** argv) {
    wchar_t temp[MAX_PATH], directory[MAX_PATH], previous[MAX_PATH];
    if (!GetTempPathW(MAX_PATH, temp) || !GetTempFileNameW(temp, L"maz", 0, directory)) return 2;
    DeleteFileW(directory);
    if (!CreateDirectoryW(directory, nullptr)) return 2;
    GetCurrentDirectoryW(MAX_PATH, previous);
    int result = 2;
    try {
        HMODULE module = GetModuleHandleW(nullptr);
        for (const auto& file : files) {
            HRSRC resource = FindResourceW(module, MAKEINTRESOURCEW(file.id), RT_RCDATA);
            if (!resource || SizeofResource(module, resource) != file.size) throw "Missing embedded asset";
            const void* data = LockResource(LoadResource(module, resource));
            if (!data) throw "Cannot read embedded asset";
            std::ofstream out(std::filesystem::path(directory) / file.name, std::ios::binary);
            out.write(static_cast<const char*>(data), file.size);
            out.close();
            if (!out) throw "Cannot extract embedded asset";
        }
        if (argc == 2 && std::strcmp(argv[1], "--check-assets") == 0) {
            result = 0;
        } else {
            if (!SetCurrentDirectoryW(directory)) throw "Cannot select asset directory";
            result = OriginalMazeMain();
            PlaySoundW(nullptr, nullptr, 0);
            closegraph();
        }
    } catch (...) {
        if (!(argc == 2 && std::strcmp(argv[1], "--check-assets") == 0))
            MessageBoxW(nullptr, L"Cannot prepare game resources.", L"MazeSolver", MB_ICONERROR);
    }
    SetCurrentDirectoryW(previous);
    for (const auto& file : files) DeleteFileW((std::filesystem::path(directory) / file.name).c_str());
    RemoveDirectoryW(directory);
    return result;
}
'''.replace('ENTRIES', ',\n'.join(entries))
    (BUILD / 'portable.cpp').write_text(wrapper, encoding='utf-8')
    subprocess.run(['rc', '/nologo', '/fo', str(BUILD / 'assets.res'), str(BUILD / 'assets.rc')], check=True)
    executable = DIST / 'MazeSolver-Astar-Windows-x64.exe'
    subprocess.run(['cl', '/nologo', '/O2', '/MT', '/EHsc', '/std:c++17', '/utf-8',
                    '/DUNICODE', '/D_UNICODE', '/D_CRT_SECURE_NO_WARNINGS', '/I' + str(header.parent),
                    str(BUILD / 'portable.cpp'), str(BUILD / 'assets.res'),
                    '/Fe:' + str(executable), '/Fo:' + str(BUILD / 'portable.obj'),
                    '/link', '/LIBPATH:' + str(candidates[0].parent)], check=True)
    subprocess.run([str(executable), '--check-assets'], check=True)
    print('Portable executable:', executable)


if __name__ == '__main__':
    main()
