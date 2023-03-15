using namespace std;

#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <map>
#include <vector>
#include <cstdlib>
#include <thread>
#include <string>
#include <locale>
#include <io.h>
#include <fcntl.h>
#include <codecvt>

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

    //std::vector<INPUT> input;
    //string unicodeString = Char.ConvertFromUtf32(str);
    INPUT singleInput; // = new INPUT[str.Length];

    WORD lastSymbol = NULL;
    int i2 = 0;

    for (int i = 0; i < str.length(); ++i)
    {
        /*if (lastSymbol != NULL && lastSymbol == (WORD)str[i]) {
            SendInput(1, singleInput, sizeof(INPUT));
            input.clear();
            i2 = 0;
        }*/
        //lastSymbol = (WORD)str[i]; // () thing needed?
        singleInput.type = INPUT_KEYBOARD;
        singleInput.ki.wScan = (WORD)str[i];
        singleInput.ki.dwFlags = KEYEVENTF_UNICODE;
        SendInput(1, &singleInput, sizeof(INPUT));
        Sleep(10);
        if (i > 0 && str[i] == str[i - 1]) {
            Sleep(40);
            //cout << "same symbol\n";
        }
        i2 += 1;
    }

    //if(input.size()>0) SendInput(input.size(), &input[0], sizeof(INPUT));
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

/*static void sendCharUnicodeTest()
{
    //[DllImport("USER32.DLL", CharSet = CharSet.Unicode)]

    / * LoadKeyboardLayout(0x0000041a, 0);
    
    cout << key << endl;
    //MapVirtualKeyEx(key, 0, 0);
    keybd_event((unsigned char)key, 0, 0, 0);
    keybd_event((unsigned char)key, 0, KEYEVENTF_KEYUP, 0); * /
    
    WORD key = 'й';
    //short key = VkKeyScan('к');

    INPUT input;
    input.type = INPUT_KEYBOARD;
    input.ki.wScan = key;
    input.ki.dwFlags = KEYEVENTF_UNICODE; // KEYEVENTF_SCANCODE

    SendInput(1, &input, sizeof(INPUT));

    input = INPUT();
    input.type = INPUT_KEYBOARD;
    input.ki.wScan = key;
    input.ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;

    SendInput(1, &input, sizeof(INPUT));
}*/

void SendInputString(const std::wstring& str)
{
    int len = str.length();
    if (len == 0) return;

    std::vector<INPUT> in(len * 2);
    ZeroMemory(&in[0], in.size() * sizeof(INPUT)); // have no idea what is this, but thanks to stakoverflow question 31305404

    int i = 0, idx = 0;
    while (i < len)
    {
        WORD ch = (WORD)str[i++];

        if ((ch < 0xD800) || (ch > 0xDFFF))
        {
            in[idx].type = INPUT_KEYBOARD;
            in[idx].ki.wScan = ch;
            in[idx].ki.dwFlags = KEYEVENTF_UNICODE;
            ++idx;

            in[idx] = in[idx - 1];
            in[idx].ki.dwFlags |= KEYEVENTF_KEYUP;
            ++idx;
        }
        else
        {
            in[idx].type = INPUT_KEYBOARD;
            in[idx].ki.wScan = ch;
            in[idx].ki.dwFlags = KEYEVENTF_UNICODE;
            ++idx;

            in[idx].type = INPUT_KEYBOARD;
            in[idx].ki.wScan = (WORD)str[i++];
            in[idx].ki.dwFlags = KEYEVENTF_UNICODE;
            ++idx;

            in[idx] = in[idx - 2];
            in[idx].ki.dwFlags |= KEYEVENTF_KEYUP;
            ++idx;

            in[idx] = in[idx - 2];
            in[idx].ki.dwFlags |= KEYEVENTF_KEYUP;
            ++idx;
        }
    }

    SendInput(in.size(), &in[0], sizeof(INPUT));
}

void inputMessage()
{
    inputKey(0x35, 200);
    //wcout << "abcd\n";
    //wcout << "This " << curText << " L \n";
    Sleep(200);

    wstring thing = L"afsdf Но да зачем";

    //wcout << typeid(curText).name() << endl;
    //wcout << typeid(thing).name() << endl;

    wcout << (curText == thing) << endl;

    SendInputString(curText); // curText
    //SendInputStr(curText); // Символы Unicode

    Sleep(500); // 50

    inputKey(0x1c, 200); // enter
    return;
}

void startTyping() {
    std::ios_base::sync_with_stdio(false);

    std::locale utf8(std::locale(), new std::codecvt_utf8_utf16<wchar_t>);
    std::wcout.imbue(utf8);

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

/*void startTyping2() {
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
}*/

void inputStart() {
    curInputThr = new thread(startTyping);
}

//InputLoopInit* inputCurInut;

//#include <typeinfo>

int main()
{
    //std::locale::global(std::locale("")); // good old friend
    /*std::ios_base::sync_with_stdio(false);

    std::locale utf8(std::locale(), new std::codecvt_utf8_utf16<wchar_t>);
    std::wcout.imbue(utf8);*/
    
    //_setmode(_fileno(stdout), _O_U16TEXT);
    //SetConsoleCP(CP_UTF8);
    bool debugMode = false;

    //std::locale::global(locale("en_US.utf8"));
    cout << "Version 1.1" << endl;
    if (RegisterHotKey(NULL, 1, MOD_ALT | MOD_NOREPEAT, 0x4E)) { wprintf(L"Hotkey 'Alt + N': Start/stopInput typing in chat (in the focused window)\n"); }
    if (RegisterHotKey(NULL, 2, MOD_CONTROL | MOD_SHIFT | MOD_NOREPEAT, 0x4E)) { wprintf(L"Hotkey 'Ctrl + Shift + N': Set new text\n"); }

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) != 0) {
        if (msg.message == WM_HOTKEY) {
            HWND curHwnd = GetForegroundWindow();
            if (msg.wParam == 1) {
                w = curHwnd;
                if (!debugMode && curText == L"") {
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

                wchar_t buffer[0x1000];
                if (!_getws_s(buffer)) cout << "!_getws_s" << endl;
                const std::wstring first = buffer;
                //_putws(first.c_str());

                curText = first;
                wcout << curText << endl; // "l " << curText[curText.size() - 1] << " e"

                wstring thing = L"ab аб";
                for (int i = 0; i < min(curText.size(), thing.size()); i++) {
                    wcout << (curText[i] == thing[i]);
                    if (curText[i] != thing[i]) wcout << " " << curText[i] << " " << thing[i];
                    wcout << endl;
                }

                //getline(wcin, curText);
                if (curText == L"\n") curText = L"";
                
                //wcout << "You entered: " << curText << endl;
                //wcout << "MAKE SURE to switch to ENG keyboard laybout\n";
                //getLanguageLayout();
            }
        }
    }
}