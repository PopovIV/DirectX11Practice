#include "input.h"

// Function to initialize interface 
HRESULT Input::Init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight) {
    HRESULT hr = S_OK;

    // Store the screen size which will be used for positioning the mouse cursor.
    m_screenWidth = screenWidth;
    m_screenHeight = screenHeight;

    // Initialize the main direct input interface.
    hr = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);

    // Initialize the direct input interface for the keyboard.
    if (SUCCEEDED(hr)) {
        hr = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
    }

    // Set the data format.  In this case since it is a keyboard we can use the predefined data format.
    if (SUCCEEDED(hr)) {
        hr = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
    }

    // Set the cooperative level of the keyboard to not share with other programs.
    if (SUCCEEDED(hr)) {
        hr = m_keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
    }

    // Now acquire the keyboard.
    if (SUCCEEDED(hr)) {
        hr = m_keyboard->Acquire();
    }

    // Initialize the direct input interface for the mouse.
    if (SUCCEEDED(hr)) {
        hr = m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
    }

    // Set the data format for the mouse using the pre-defined mouse data format.
    if (SUCCEEDED(hr)) {
        hr = m_mouse->SetDataFormat(&c_dfDIMouse);
    }

    // Set the cooperative level of the mouse to share with other programs.
    if (SUCCEEDED(hr)) {
        hr = m_mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
    }

    // Acquire the mouse.
    if (SUCCEEDED(hr)) {
        hr = m_mouse->Acquire();
    }

    return hr;

}

// Function to check if left key is pressed
bool Input::IsLeftPressed() {
    // Do a bitwise and on the keyboard state to check if the key is currently being pressed.
    if (m_keyboardState[DIK_LEFT] || m_keyboardState[DIK_A] & 0x80)
        return true;


    return false;
}

// Function to check if right key is pressed
bool Input::IsRightPressed() {
    // Do a bitwise and on the keyboard state to check if the key is currently being pressed.
    if (m_keyboardState[DIK_RIGHT] || m_keyboardState[DIK_D] & 0x80)
        return true;

    return false;
}

// Function to check if up key is pressed
bool Input::IsUpPressed() {
    // Do a bitwise and on the keyboard state to check if the key is currently being pressed.
    if (m_keyboardState[DIK_UP] || m_keyboardState[DIK_W] & 0x80)
        return true;

    return false;
}

// Function to check if down key is pressed
bool Input::IsDownPressed() {
    // Do a bitwise and on the keyboard state to check if the key is currently being pressed.
    if (m_keyboardState[DIK_DOWN] || m_keyboardState[DIK_S] & 0x80)
        return true;

    return false;
}

// Resize function
void Input::Resize(int screenWidth, int screenHeight) {
    m_screenWidth = screenWidth;
    m_screenHeight = screenHeight;
}

// Function to realese interface
void Input::Release() {
    // Release the mouse.
    if (m_mouse) {
        m_mouse->Unacquire();
        m_mouse->Release();
        m_mouse = nullptr;
    }

    // Release the keyboard.
    if (m_keyboard) {
        m_keyboard->Unacquire();
        m_keyboard->Release();
        m_keyboard = nullptr;
    }

    // Release the main interface to direct input.
    if (m_directInput) {
        m_directInput->Release();
        m_directInput = nullptr;
    }
}

// Function to read the current state into state buffers
bool Input::Frame() {
    bool result;

    // Read the current state of the keyboard.
    result = ReadKeyboard();
    if (!result)
        return false;

    // Read the current state of the mouse.
    result = ReadMouse();
    if (!result)
        return false;

    return true;
}

// Function to read the state of keyboard
bool Input::ReadKeyboard() {
    HRESULT result;

    // Read the keyboard device.
    result = m_keyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
    if (FAILED(result)) {
        // If the keyboard lost focus or was not acquired then try to get control back.
        if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
            m_keyboard->Acquire();
        else
            return false;
    }

    return true;
}

// Function to read the state of mouse
bool Input::ReadMouse() {
    HRESULT result;

    // Read the mouse device.
    result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);
    if (FAILED(result)) {
        // If the mouse lost focus or was not acquired then try to get control back.
        if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
            m_mouse->Acquire();
        else
            return false;
    }

    return true;
}

// Function to check if mouse is used
XMFLOAT3 Input::IsMouseUsed() {
    if (m_mouseState.rgbButtons[0] || m_mouseState.rgbButtons[1] || m_mouseState.rgbButtons[2] & 0x80)
        return XMFLOAT3((float)m_mouseState.lX, (float)m_mouseState.lY, (float)m_mouseState.lZ);

    return XMFLOAT3(0.0f, 0.0f, 0.0f);
};
