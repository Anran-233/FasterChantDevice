#include "key.h"
#include "windows.h"
#include "tlhelp32.h"

#include <QClipboard>
#include <QThreadPool>
#include <QtConcurrent>
#include <QApplication>

std::wstring str300name(L"300.exe");
CKey g_key;

/// 触发热键是否处于按下状态
bool g_bKey{ false };
/// 是否已安装键盘钩子
bool g_bHook = false;
/// 键盘钩子
HHOOK g_keyHook = NULL;
/// 焦点窗口句柄
HWND g_focusHwnd = NULL;
/// 是否正在发送消息
bool g_bSend = false;
/// 屏蔽按键列表
QSet<DWORD> g_shieldKeys;
/// 连发模式进行符
bool g_bContinuityRun = false;
/// 空格键状态
bool g_bSpace = false;

/// 键盘事件处理函数
LRESULT CALLBACK keyProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    KBDLLHOOKSTRUCT *pkbhs = (KBDLLHOOKSTRUCT *)lParam;
    if (g_bKey) {
        if (g_bSend) return CallNextHookEx(g_keyHook, nCode, wParam, lParam);
        if (pkbhs->vkCode == VK_SPACE) {
            g_bSpace = wParam == WM_KEYDOWN;
            return 1;
        }
        if (!g_shieldKeys.contains(pkbhs->vkCode)) return CallNextHookEx(g_keyHook, nCode, wParam, lParam);
        if (wParam == WM_KEYUP) g_key.get(pkbhs->vkCode);   // 只处理按键释放信号
        return 1;   // 返回1表示截取消息不再传递,返回0表示不作处理,消息继续传递
    }
    else {
        if (wParam == WM_KEYDOWN && (pkbhs->vkCode == (DWORD)g_keyValue1 || pkbhs->vkCode == (DWORD)g_keyValue2)) {
            if (!g_key.id()) return CallNextHookEx(g_keyHook, nCode, wParam, lParam);
            // 初始化
            if (!g_key.init()) return CallNextHookEx(g_keyHook, nCode, wParam, lParam);
            if (!g_bSeniorShield) PostMessage(g_focusHwnd, WM_KEYDOWN, pkbhs->vkCode, NULL);
            // 显示
            g_bKey = true;
            g_key.switchPage(0);
            g_key.show();
            // 设置线程监测器
            g_key.setMonitor();
            return 1;
        }
    }
    return CallNextHookEx(g_keyHook, nCode, wParam, lParam);
}

/// 连发消息
void ContinuitySendMessage(int id, int index)
{
    static QThreadPool pool;
    if (pool.activeThreadCount()) return;
    g_bContinuityRun = true;
    QtConcurrent::run(&pool, [&](int tid, int tindex, int time) {
        for (int i = 0; i <= 9; ++i) {
            if (!g_bContinuityRun) return;
            if (GetForegroundWindow() != g_focusHwnd) return;
            if (!g_scheme.contains(tid)) return;
            const QString& str = g_scheme[tid][tindex][i];
            if (str.isEmpty()) continue;
            // 将文本放入剪贴板
            g_key.clipboard(str);
            g_bSend = true;
            // 打开聊天框
            if (!g_bSeniorMode) {
                if (g_bPublic) keybd_event(VK_RSHIFT, 0, 0, 0);
                keybd_event(VK_RETURN, 0, 0, 0);
                keybd_event(VK_RETURN, 0, KEYEVENTF_KEYUP, 0);
                if (g_bPublic) keybd_event(VK_RSHIFT, 0, KEYEVENTF_KEYUP, 0);
            }
            // 粘贴 Ctrl + V
            keybd_event(VK_RCONTROL, 0, 0, 0);
            keybd_event('V', 0, 0, 0);
            keybd_event('V', 0, KEYEVENTF_KEYUP, 0);
            keybd_event(VK_RCONTROL, 0, KEYEVENTF_KEYUP, 0);
            // 发送消息
            keybd_event(VK_RETURN, 0, 0, 0);
            keybd_event(VK_RETURN, 0, KEYEVENTF_KEYUP, 0);
            g_bSend = false;

            for (int j = i + 1; j <= 10; ++j) {
                if (j == 10) return;
                else if (!g_scheme[tid][tindex][j].isEmpty()) break;
            }
            for (int t = 0, tend = time * 10; t < tend; ++t) {
                if (!g_bContinuityRun) return;
                if (time != g_seniorContinuity) return;
                Sleep(100);
            }
        }
    }, id, index, g_seniorContinuity);
}

//卸载钩子
void UnHook()
{
    UnhookWindowsHookEx(g_keyHook);
    g_bHook = false;
}

//安装钩子,调用该函数即安装钩子
void SetHook()
{
    // 设置屏蔽按键列表(数字键、字母键、触发键、退格键、空格键)
    for (DWORD i = 48; i <= 57; ++i) g_shieldKeys.insert(i);
    for (DWORD i = 65; i <= 90; ++i) g_shieldKeys.insert(i);
    for (DWORD i = 96; i <= 105; ++i) g_shieldKeys.insert(i);
    for (const auto& i : GetKeyList()) g_shieldKeys.insert(i);
    g_shieldKeys.insert(VK_BACK);
    g_shieldKeys.insert(VK_SPACE);
    // 设置键盘钩子
    g_keyHook = SetWindowsHookEx(WH_KEYBOARD_LL, keyProc, GetModuleHandle(NULL), 0);
    g_bHook = true;
}

// 初始化
const int &CKey::id()
{
    m_id = getid();
    return m_id;
}

bool CKey::init()
{
    // 初始化变量
    m_page = 0;
    m_input.clear();
    g_focusHwnd = 0;

    // 获取焦点窗口句柄
    HWND hWnd = GetForegroundWindow();
    if (!hWnd) return false;
    if (g_bOnly300) {
        DWORD dwPid = -1;
        GetWindowThreadProcessId(hWnd, &dwPid);
        HANDLE hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);    // 系统快照
        if (hSnapshot){
            PROCESSENTRY32 lppe;
            lppe.dwSize = sizeof(lppe);
            if (::Process32First(hSnapshot, &lppe)){
                do if (lppe.th32ProcessID == dwPid) if (str300name == lppe.szExeFile) { g_focusHwnd = hWnd; break; }
                while (::Process32Next(hSnapshot, &lppe)); //查找下一个进程
            }
        }
    }
    else g_focusHwnd = hWnd;
    if (!g_focusHwnd) return false;

    // 获取焦点窗口大小位置
    RECT rect;
    GetClientRect(hWnd, &rect);
    POINT pos{ rect.left, rect.top };
    ClientToScreen(hWnd, &pos);
    g_key.sendRect(pos.x, pos.y, rect.right - rect.left, rect.bottom - rect.top);

    return true;
}

// 设置监测器
void CKey::setMonitor()
{
    static QThreadPool pool;
    if (pool.maxThreadCount() - pool.activeThreadCount() < 2) pool.setMaxThreadCount(pool.activeThreadCount() + 2);
    // 监测焦点窗口是否切换
    QtConcurrent::run(&pool,[]{
        while (g_bKey) {
            if (GetForegroundWindow() != g_focusHwnd) {
                g_key.hide();
                g_bKey = false;
                break;
            }
            else for(size_t&& i = 0; i < 5 && g_bKey; ++i) Sleep(10);
        }
    });
    // 监测面板是否已退出【保险处理】
    QtConcurrent::run(&pool,[]{
        while (g_bKey) {
            if (!g_bKey) {
                g_key.hide();
                break;
            }
            for(size_t&& i = 0; i < 5 && g_bKey; ++i) Sleep(10);
        }
    });
}

// 获取按键输入
void CKey::get(int key)
{
    // 关闭面板
    if (key == g_keyValue1 || key == g_keyValue2) {
        g_key.hide();
        g_bKey = false;
        return;
    }
    // 【空格键】设置连发功能
    if (g_bSpace) {
        int num = 0;
        if (key < '0' || key > '9') {
            if (key < 96 || key > 105) return;
            else num = key - 96;
        }
        else num = key - 48;
        g_key.setContinuity(num);
        return;
    }
    // 0~9 (ASCII)48~57 (num key)96~105
    // A~Z (ASCII)65~90
    if (m_page == 0) {  // 首页
        int num = 0;
        if (key < '0' || key > '9') {
            if (key < 96 || key > 105) return;
            else num = key - 96;
        }
        else num = key - 48;
        if (num == 0) { // 切换搜索面板
            g_bContinuityRun = false;
            m_page = -1;
            switchPage(-1);
        }
        else if (g_seniorContinuity) {  // 连发模式
            ContinuitySendMessage(m_id, num);
        }
        else if (g_scheme[m_id].type){   // 切换二级面板
            m_page = num;
            switchPage(num);
        }
        else sendMessage(g_scheme[m_id][0][num]);   // 发送消息
    }
    else if (m_page == -1) {    // 搜索页
        if (m_input.size()){
            const bool& bId = m_input.at(0) < 60;
            if (bId) {
                if (key == VK_BACK) {
                    m_input.clear();
                    switchPage(-1);
                    return;
                }
                int num = 0;
                if (key < '0' || key > '9') {
                    if (key < 96 || key > 105) return;
                    else num = key - 96;
                }
                else num = key - 48;
                const int& newid = m_input.toInt() * 10 + num;
                if (g_scheme.contains(newid)) {
                    switchId(newid);
                    m_id = newid;
                    m_page = 0;
                    m_input.clear();
                }
            }
            else {
                if (key == VK_BACK) {
                    if (m_input.size() == 1) m_input.clear(), switchPage(-1);
                    else {
                        m_input = m_input.left(m_input.size() - 1);
                        lookupName(m_input, m_list = g_scheme.find(m_input));
                    }
                    return;
                }
                if (key >= 'A' && key <= 'Z') {
                    m_input.append(key);
                    lookupName(m_input, m_list = g_scheme.find(m_input));
                    return;
                }
                int num = 0;
                if (key < '0' || key > '9') {
                    if (key < 96 || key > 105) return;
                    else num = key - 96;
                }
                else num = key - 48;
                const int& newid = m_list[num]->id[0] * 10 + m_list[num]->id[1];
                switchId(newid);
                m_id = newid;
                m_page = 0;
                m_input.clear();
            }
        }
        else {
            int num = 0;
            if (key < 'A' || key > 'Z') {
                if (key < '0' || key > '9') {
                    if (key < 96 || key > 105) return;
                    else num = key - 96;
                }
                else num = key - 48;
            }
            else m_input.append(key);
            if (m_input.size()) {   // name
                lookupName(m_input, m_list = g_scheme.find(m_input));
            }
            else {  // id
                m_input.append(num + 48);
                lookupId(num, m_list = g_scheme.find(num));
            }
        }
    }
    else {  // 二级面板
        int num = 0;
        if (key < '0' || key > '9') {
            if (key < 96 || key > 105) return;
            else num = key - 96;
        }
        else num = key - 48;
        if (!g_scheme[m_id][m_page][num].isEmpty()) sendMessage(g_scheme[m_id][m_page][num]);
    }
}

// 发送消息
void CKey::sendMessage(const QString &str)
{
    g_bSend = true;
    // 将文本放入剪贴板
    QApplication::clipboard()->setText(str);
    // 打开聊天框
    if (!g_bSeniorMode) {
        if (g_bPublic) keybd_event(VK_RSHIFT, 0, 0, 0);
        keybd_event(VK_RETURN, 0, 0, 0);
        keybd_event(VK_RETURN, 0, KEYEVENTF_KEYUP, 0);
        if (g_bPublic) keybd_event(VK_RSHIFT, 0, KEYEVENTF_KEYUP, 0);
    }
    // 粘贴 Ctrl + V
    keybd_event(VK_RCONTROL, 0, 0, 0);
    keybd_event('V', 0, 0, 0);
    keybd_event('V', 0, KEYEVENTF_KEYUP, 0);
    keybd_event(VK_RCONTROL, 0, KEYEVENTF_KEYUP, 0);
    // 发送消息
    keybd_event(VK_RETURN, 0, 0, 0);
    keybd_event(VK_RETURN, 0, KEYEVENTF_KEYUP, 0);
    g_bSend = false;
    // 切回首页
    if (g_bSeniorBack) {
        m_page = 0;
        switchPage(0);
    }
}
