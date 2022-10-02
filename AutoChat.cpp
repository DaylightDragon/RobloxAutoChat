using namespace std;

#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <map>
#include <vector>
#include <cstdlib>
#include <thread>
#include <string>

HWND w;
wstring curText = L"";

void chatLetter(char c) {
    PostMessage(w, WM_CHAR, c, 0);
}

void chatText(string s, int delay) {
    for (char& c : s) {
        chatLetter(c);
        Sleep(delay);
    }
}

void SendInputStr(const std::wstring& str) // in C++11, use std::u16string instead... // wstring before
{
    if (str.empty()) return;

    std::vector<INPUT> input(str.length());

    for (int i = 0; i < str.length(); ++i)
    {
        input[i].type = INPUT_KEYBOARD;
        input[i].ki.wScan = (WORD)str[i];
        input[i].ki.dwFlags = KEYEVENTF_UNICODE;
        Sleep(10); // just in case
    }

    SendInput(input.size(), &input[0], sizeof(INPUT));
}

void keyPress(WORD keyCode)
{
    INPUT input;
    input.type = INPUT_KEYBOARD;
    input.ki.wScan = keyCode;
    input.ki.dwFlags = KEYEVENTF_SCANCODE; // KEYEVENTF_SCANCODE

    SendInput(1, &input, sizeof(INPUT));
}

void keyRelease(WORD keyCode)
{
    INPUT input;
    input.type = INPUT_KEYBOARD;
    input.ki.wScan = keyCode;
    input.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
    
    SendInput(1, &input, sizeof(INPUT));
}

//[DllImport("user32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
void press()
{
    WORD slash = 0x35;
    //Sleep(500); // hold key for x milliseconds
    keyPress(slash); // press key // 0x35
    wcout << "Pressed\n";
    Sleep(10); // hold key for x milliseconds
    keyRelease(slash); // release key
    wcout << "Released\n";
    Sleep(1000);
    keybd_event(VK_RETURN, 0x9C, 0, 0);
    Sleep(100);
    keybd_event(VK_RETURN, 0x9C, KEYEVENTF_KEYUP, 0);
    wcout << "Sent Return\n";
    return;
    //chatText(curText, 10);
    wcout << "Sent text\n";
    return;
}

void inputKey(WORD key, int delay) {
    keyPress(key);
    Sleep(delay);
    keyRelease(key);
    Sleep(10);
}

thread* curInputThr;
boolean stopInput = true;

void inputMessage()
{
    inputKey(0x35, 200);
    //wcout << "abcd\n";
    //wcout << "This " << curText << " L \n";
    Sleep(200);

    SendInputStr(curText); // Символы Unicode

    Sleep(50);

    inputKey(0x1c, 200); // enter
    return;
}

void startTyping() {
    Sleep(500);
    while (!stopInput) {
    //for (int i = 0x1c; i <= 0x1d; i++) { // 5A
        //if (stopInput) return;
        //key = i;
        //wcout << std::hex << key << endl;
        //wcout << std::dec;
        inputMessage();
        // 8000
        for (int i = 0; i < 50; i++) {
            //Sleep(160);
            Sleep(40);
            if (stopInput) return;
        }
    }
}

void getLanguageLayout() {
    GUITHREADINFO Gti;
    ::ZeroMemory(&Gti, sizeof(GUITHREADINFO));
    Gti.cbSize = sizeof(GUITHREADINFO);
    ::GetGUIThreadInfo(0, &Gti);
    DWORD dwThread = ::GetWindowThreadProcessId(Gti.hwndActive, 0);
    HKL lang = ::GetKeyboardLayout(dwThread);
    wcout << lang << endl;
}

void startTyping2() {
    wcout << "MAKE SURE to switch to ENG keyboard laybout\n";
    Sleep(500);
    SendInputStr(curText);

    //inputKey((WORD) 'й' , 100);

    return;
    //while (!stopInput) {
    for (int i = 0x1c; i <= 0xaaa; i++) { // 5A
        if (stopInput) return;
            wcout << std::hex << i << endl;
            wcout << std::dec;
            inputKey(i, 200);
    }
}

void inputStart() {
    curInputThr = new thread(startTyping);
}

//InputLoopInit* inputCurInut;

int main()
{
    //std::locale::global(locale("en_US.utf8"));
    if (RegisterHotKey(NULL, 1, MOD_ALT | MOD_NOREPEAT, 0x4E)) { wprintf(L"Hotkey 'Alt + N': Start/stopInput typing in chat (in the focused window)\n"); }
    if (RegisterHotKey(NULL, 2, MOD_CONTROL | MOD_SHIFT | MOD_NOREPEAT, 0x4E)) { wprintf(L"Hotkey 'Ctrl + Shift + N': Set new text\n"); }

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) != 0) {
        if (msg.message == WM_HOTKEY) {
            HWND curHwnd = GetForegroundWindow();
            if (msg.wParam == 1) {
                w = curHwnd;
                if (curText == L"") {
                    wcout << "Please, enter the string first\n";
                }
                else if (stopInput) {
                    stopInput = false;
                    wcout << "Starting the input loop...\n";
                    inputStart();
                }
                else {
                    stopInput = true;
                    wcout << "Stopped\n";
                    //InputLoopInit* unit = new InputLoopInit;
                    //inputCurInut = unit;
                    //unit->start();
                }
                /*active = !active;
                if (!active) {
                    thread* th = new thread(startTyping);
                    //typingThread = th;
                }*/
                
            }
            else if (msg.wParam == 2) {
                wcout << "Enter the text (Eng only):\n";
                getline(wcin, curText);
                if (curText == L"\n") curText = L"";
                
                //wcout << "You entered: " << curText << endl;
                //wcout << "MAKE SURE to switch to ENG keyboard laybout\n";
                //getLanguageLayout();
            }
        }
    }
}