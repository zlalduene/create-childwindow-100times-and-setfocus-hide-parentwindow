#include <Windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#define TIMER_ID 1
int i = 0;
DWORD g_dwOriginalVolume;
extern "C" {
    __declspec(dllexport) void __stdcall PlayMusic();
}
TCHAR g_szMusicFilePath1[] = TEXT("D:\\浏览器下载\\hulu.wav");

void SetSystemVolume(DWORD dwVolume);


LRESULT CALLBACK ChildWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE:
        break;
    case WM_CLOSE:
        ShowWindow(hwnd, SW_HIDE);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE:
        PlayMusic();
        SetTimer(hwnd, TIMER_ID, 1000, NULL);
        break;

    case WM_TIMER:
        if (wParam == TIMER_ID) {
            HWND childWnd = CreateWindow(
                L"ChildWindowClass",
                L"子窗口",
                WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX,
                50 + i * 20, 50 + i * 20,700, 500,
                hwnd,
                NULL,
                (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
                NULL
            );
            if (childWnd == NULL) {
                MessageBox(hwnd, L"无法创建子窗口！", L"错误", MB_OK | MB_ICONERROR);
            }
            SetForegroundWindow(childWnd);

            // 将子窗口聚焦
            SetFocus(childWnd);
            SetSystemVolume(0xFFFF);
            i = (i + 1) % 100; // 重置i的值，避免无限创建窗口
        }
        break;

    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    WNDCLASS wndcls, childWndCls;

    // 注册主窗口类
    wndcls.cbClsExtra = 0;
    wndcls.cbWndExtra = 0;
    wndcls.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndcls.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndcls.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndcls.hInstance = hInstance;
    wndcls.lpfnWndProc = MainWndProc;
    wndcls.lpszClassName = L"MainWindowClass";
    wndcls.lpszMenuName = NULL;
    wndcls.style = CS_HREDRAW | CS_VREDRAW;
    RegisterClass(&wndcls);

    // 注册子窗口类
    childWndCls.cbClsExtra = 0;
    childWndCls.cbWndExtra = 0;
    childWndCls.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    childWndCls.hCursor = LoadCursor(NULL, IDC_ARROW);
    childWndCls.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    childWndCls.hInstance = hInstance;
    childWndCls.lpfnWndProc = ChildWndProc;
    childWndCls.lpszClassName = L"ChildWindowClass";
    childWndCls.lpszMenuName = NULL;
    childWndCls.style = CS_HREDRAW | CS_VREDRAW;
    RegisterClass(&childWndCls);

    HWND hwnd = CreateWindow(
        L"MainWindowClass", L"我的第一个窗口",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, hInstance, NULL);

    if (hwnd == NULL) {
        return 0;
    }

    ShowWindow(hwnd,1);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
void __stdcall PlayMusic() {
    // 使用PlaySound函数播放音乐
    // 第一个参数是音乐文件的路径，第二个参数是调用PlaySound的方式
    PlaySound(g_szMusicFilePath1, NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);


}
void SetSystemVolume(DWORD dwVolume) {
    // 获取系统默认音频设备
    HMIXER hMixer;
    MIXERLINE ml;
    MIXERCONTROL mc;
    MIXERLINECONTROLS mlc;
    MIXERCONTROLDETAILS mcd;
    DWORD dwControlID;
    MIXERCONTROLDETAILS_UNSIGNED value;

    if (mixerOpen(&hMixer, 0, 0, 0, MIXER_OBJECTF_MIXER) == MMSYSERR_NOERROR) {
        ml.cbStruct = sizeof(MIXERLINE);
        ml.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;

        if (mixerGetLineInfo(reinterpret_cast<HMIXEROBJ>(hMixer), &ml, MIXER_GETLINEINFOF_COMPONENTTYPE) == MMSYSERR_NOERROR) {
            mlc.cbStruct = sizeof(MIXERLINECONTROLS);
            mlc.dwLineID = ml.dwLineID;
            mlc.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
            mlc.cControls = 1;
            mlc.cbmxctrl = sizeof(MIXERCONTROL);
            mlc.pamxctrl = &mc;

            if (mixerGetLineControls(reinterpret_cast<HMIXEROBJ>(hMixer), &mlc, MIXER_GETLINECONTROLSF_ONEBYTYPE) == MMSYSERR_NOERROR) {
                dwControlID = mc.dwControlID;
                mcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
                mcd.dwControlID = dwControlID;
                mcd.cChannels = 1;
                mcd.cMultipleItems = 0;
                mcd.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
                mcd.paDetails = &value;

                value.dwValue = dwVolume;

                if (mixerSetControlDetails(reinterpret_cast<HMIXEROBJ>(hMixer), &mcd, MIXER_SETCONTROLDETAILSF_VALUE) != MMSYSERR_NOERROR) {
                    // 设置音量失败
                }
            }
        }

        mixerClose(hMixer);
    }
}