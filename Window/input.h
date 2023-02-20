// Input.h - class provides high speed method of interfacing with input devics

#define DIRECTINPUT_VERSION 0x0800

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include <dinput.h>
#include <directxmath.h>

using namespace DirectX;

class Input {
public:
    // Function to initialize interface 
    HRESULT Init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight);
    // Function to realese interface
    void Realese();
    // Function to read the current state into state buffers
    bool Frame();

    // Function to check if mouse is used
    XMFLOAT3 IsMouseUsed();
    // Resize function
    void Resize(int screenWidth, int screenHeight);

private:
    // Function to read the state of keyboard
    bool ReadKeyboard();
    // Function to read the state of mouse
    bool ReadMouse();

    IDirectInput8* m_directInput = nullptr;
    IDirectInputDevice8* m_keyboard = nullptr;
    IDirectInputDevice8* m_mouse = nullptr;

    unsigned char m_keyboardState[256];
    DIMOUSESTATE m_mouseState = {};

    int m_screenWidth = 0, m_screenHeight = 0;
};
