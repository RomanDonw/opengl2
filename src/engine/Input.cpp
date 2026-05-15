#include "Input.hpp"

#include "Events.hpp"
#include "Window.hpp"

#include <algorithm>
#include <cmath>

bool Input::isValidKey(int keycode) { return keycode >= 0 && keycode < KEY_COUNT; }

bool Input::isValidMouseButton(int button) { return button >= 0 && button < MOUSE_BUTTON_COUNT; }

bool Input::isGameplayAllowed()
{
    if (!window || !HasFocus()) return false;
    if (contextStack.empty()) return true;
    return contextStack.back() == InputContext::Gameplay;
}

bool Input::isKeyboardAllowed() { return isGameplayAllowed() && !uiBlocksKeyboard; }

bool Input::isMouseAllowed() { return isGameplayAllowed() && !uiBlocksMouse; }

void Input::keyCallback(GLFWwindow *w, int key, int scancode, int action, int mods)
{
    if (isValidKey(key)) keysDown[key] = action != GLFW_RELEASE;

    KeyEvent event{key, action, mods};
    Events::DispatchKey(event);

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) closeRequested = true;
}

void Input::mouseButtonCallback(GLFWwindow *w, int button, int action, int mods)
{
    if (isValidMouseButton(button)) mouseDown[button] = action == GLFW_PRESS;

    Events::DispatchMouseButton({button, action, mods});
}

void Input::cursorPosCallback(GLFWwindow *w, double x, double y)
{
    const double dx = x - mouseX;
    const double dy = y - mouseY;
    mouseX = x;
    mouseY = y;
    Events::DispatchMouseMove({x, y, dx, dy});
}

void Input::scrollCallback(GLFWwindow *w, double xoff, double yoff)
{
    scrollDelta.x += static_cast<float>(xoff);
    scrollDelta.y += static_cast<float>(yoff);
    Events::DispatchScroll({xoff, yoff});
}

void Input::charCallback(GLFWwindow *w, unsigned int codepoint)
{
    Events::DispatchChar({codepoint});
}

void Input::updateKeyboardState()
{
    if (!window) return;

    for (int i = 0; i < KEY_COUNT; ++i)
    {
        if (!keysDown[i] && !keysDownPrev[i]) continue;
        keysDown[i] = glfwGetKey(window, i) == GLFW_PRESS;
    }
}

void Input::updateMouseState()
{
    if (!window) return;
    glfwGetCursorPos(window, &mouseX, &mouseY);
}

void Input::updateGamepadState()
{
    if (!IsGamepadConnected(GLFW_JOYSTICK_1))
    {
        for (int i = 0; i < GAMEPAD_BUTTON_COUNT; ++i) gamepadDown[i] = false;
        return;
    }

    int count = 0;
    const unsigned char *buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &count);
    if (!buttons) return;

    const int n = std::min(count, GAMEPAD_BUTTON_COUNT);
    for (int i = 0; i < GAMEPAD_BUTTON_COUNT; ++i) gamepadDown[i] = i < n && buttons[i] == GLFW_PRESS;
}

void Input::applyMouseCapture(bool captured)
{
    if (!window) return;

    mouseCaptured = captured;

    if (captured)
    {
        int w, h;
        glfwGetWindowSize(window, &w, &h);
        const double cx = w / 2.0;
        const double cy = h / 2.0;
        glfwSetCursorPos(window, cx, cy);
        lastMouseX = cx;
        lastMouseY = cy;
        mouseX = cx;
        mouseY = cy;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    else
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

void Input::updateMouseCapture()
{
    if (!window) return;

    if (uiBlocksMouse)
    {
        if (mouseCaptured) applyMouseCapture(false);
        return;
    }

    const bool buttonDown = glfwGetMouseButton(window, captureButton) == GLFW_PRESS;

    if (captureToggle)
    {
        if (buttonDown && !captureToggleArmed)
        {
            captureToggleArmed = true;
            applyMouseCapture(!mouseCaptured);
        }
        else if (!buttonDown)
        {
            captureToggleArmed = false;
        }
    }
    else
    {
        if (buttonDown && !mouseCaptured) applyMouseCapture(true);
        else if (!buttonDown && mouseCaptured) applyMouseCapture(false);
    }
}

void Input::Init(GLFWwindow *w)
{
    window = w;
    closeRequested = false;
    scrollDelta = glm::vec2(0.0f);
    mouseLookDelta = glm::vec2(0.0f);
    mouseCaptured = false;
    captureToggleArmed = false;
    uiBlocksKeyboard = false;
    uiBlocksMouse = false;
    contextStack = {InputContext::Gameplay};
    actions.clear();
    axes.clear();

    for (int i = 0; i < KEY_COUNT; ++i) keysDown[i] = keysDownPrev[i] = false;
    for (int i = 0; i < MOUSE_BUTTON_COUNT; ++i) mouseDown[i] = mouseDownPrev[i] = false;
    for (int i = 0; i < GAMEPAD_BUTTON_COUNT; ++i) gamepadDown[i] = gamepadDownPrev[i] = false;

    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetCharCallback(window, charCallback);

    int winW, winH;
    glfwGetWindowSize(window, &winW, &winH);
    lastMouseX = mouseX = winW / 2.0;
    lastMouseY = mouseY = winH / 2.0;
    glfwSetCursorPos(window, lastMouseX, lastMouseY);

    SetupDefaultBindings();
}

void Input::SetupDefaultBindings()
{
    BindKey("MoveForward", GLFW_KEY_W);
    BindKey("MoveBack", GLFW_KEY_S);
    BindKey("MoveLeft", GLFW_KEY_A);
    BindKey("MoveRight", GLFW_KEY_D);
    BindKey("Jump", GLFW_KEY_SPACE);
    BindKey("Sprint", GLFW_KEY_LEFT_SHIFT);
    BindKey("Crouch", GLFW_KEY_LEFT_CONTROL);
    BindKey("Fire", GLFW_MOUSE_BUTTON_LEFT);
    BindKey("Aim", GLFW_MOUSE_BUTTON_RIGHT);

    BindAxisKeys("MoveX", GLFW_KEY_A, GLFW_KEY_D);
    BindAxisKeys("MoveZ", GLFW_KEY_W, GLFW_KEY_S);

    BindAxisGamepad("MoveX", GLFW_JOYSTICK_1, 0);
    BindAxisGamepad("MoveZ", GLFW_JOYSTICK_1, 1, true);
    BindAxisGamepad("LookX", GLFW_JOYSTICK_1, 2);
    BindAxisGamepad("LookY", GLFW_JOYSTICK_1, 3, true);
}

void Input::Shutdown()
{
    if (window && mouseCaptured) glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    window = nullptr;
    mouseCaptured = false;
    actions.clear();
    axes.clear();
}

void Input::Poll()
{
    if (!window) return;

    glfwPollEvents();
    updateKeyboardState();
    updateMouseState();
    updateGamepadState();
    updateMouseCapture();
}

void Input::EndFrame()
{
    for (int i = 0; i < KEY_COUNT; ++i) keysDownPrev[i] = keysDown[i];
    for (int i = 0; i < MOUSE_BUTTON_COUNT; ++i) mouseDownPrev[i] = mouseDown[i];
    for (int i = 0; i < GAMEPAD_BUTTON_COUNT; ++i) gamepadDownPrev[i] = gamepadDown[i];
}

void Input::UpdateMouseLook()
{
    mouseLookDelta = glm::vec2(0.0f);

    if (!window || !isMouseAllowed()) return;
    if (glfwGetMouseButton(window, captureButton) != GLFW_PRESS) return;

    mouseLookDelta.x = -glm::radians(static_cast<float>(mouseX - lastMouseX) * mouseSensitivity.x);
    mouseLookDelta.y = -glm::radians(static_cast<float>(mouseY - lastMouseY) * mouseSensitivity.y);

    lastMouseX = mouseX;
    lastMouseY = mouseY;
}

void Input::SetUIBlocking(bool keyboard, bool mouse)
{
    uiBlocksKeyboard = keyboard;
    uiBlocksMouse = mouse;
}

void Input::PushContext(InputContext context) { contextStack.push_back(context); }

void Input::PopContext()
{
    if (contextStack.size() > 1) contextStack.pop_back();
}

InputContext Input::GetContext() { return contextStack.back(); }

bool Input::HasFocus() { return Window::IsFocused(); }

bool Input::IsKeyDownRaw(int keycode)
{
    if (!isValidKey(keycode)) return false;
    return keysDown[keycode];
}

bool Input::IsKeyJustPressedRaw(int keycode)
{
    if (!isValidKey(keycode)) return false;
    return keysDown[keycode] && !keysDownPrev[keycode];
}

bool Input::IsKeyJustReleasedRaw(int keycode)
{
    if (!isValidKey(keycode)) return false;
    return !keysDown[keycode] && keysDownPrev[keycode];
}

bool Input::IsKeyDown(int keycode)
{
    if (!isKeyboardAllowed()) return false;
    return IsKeyDownRaw(keycode);
}

bool Input::IsKeyPressed(int keycode) { return IsKeyDown(keycode); }

bool Input::IsKeyJustPressed(int keycode)
{
    if (!isKeyboardAllowed()) return false;
    return IsKeyJustPressedRaw(keycode);
}

bool Input::IsKeyJustReleased(int keycode)
{
    if (!isKeyboardAllowed()) return false;
    return IsKeyJustReleasedRaw(keycode);
}

bool Input::IsMouseButtonDownRaw(int button)
{
    if (!isValidMouseButton(button)) return false;
    return mouseDown[button];
}

bool Input::IsMouseButtonJustPressedRaw(int button)
{
    if (!isValidMouseButton(button)) return false;
    return mouseDown[button] && !mouseDownPrev[button];
}

bool Input::IsMouseButtonJustReleasedRaw(int button)
{
    if (!isValidMouseButton(button)) return false;
    return !mouseDown[button] && mouseDownPrev[button];
}

bool Input::IsMouseButtonDown(int button)
{
    if (!isMouseAllowed()) return false;
    return IsMouseButtonDownRaw(button);
}

bool Input::IsMouseButtonPressed(int button) { return IsMouseButtonDown(button); }

bool Input::IsMouseButtonJustPressed(int button)
{
    if (!isMouseAllowed()) return false;
    return IsMouseButtonJustPressedRaw(button);
}

bool Input::IsMouseButtonJustReleased(int button)
{
    if (!isMouseAllowed()) return false;
    return IsMouseButtonJustReleasedRaw(button);
}

glm::dvec2 Input::GetMousePosition() { return glm::dvec2(mouseX, mouseY); }

glm::vec2 Input::GetMousePositionNormalized()
{
    if (!window) return glm::vec2(0.0f);

    int w, h;
    glfwGetWindowSize(window, &w, &h);
    if (w <= 0 || h <= 0) return glm::vec2(0.0f);

    return glm::vec2(static_cast<float>(mouseX / w), static_cast<float>(mouseY / h));
}

glm::vec2 Input::GetMouseLookDelta() { return mouseLookDelta; }

glm::vec2 Input::GetScrollDelta() { return scrollDelta; }

void Input::ClearScrollDelta() { scrollDelta = glm::vec2(0.0f); }

void Input::SetMouseCaptureButton(int button) { captureButton = button; }

void Input::SetMouseCaptureToggle(bool toggle)
{
    captureToggle = toggle;
    captureToggleArmed = false;
}

void Input::SetMouseSensitivity(glm::vec2 sensitivity) { mouseSensitivity = sensitivity; }

void Input::SetMouseCaptured(bool captured)
{
    if (mouseCaptured == captured) return;
    applyMouseCapture(captured);
}

bool Input::IsMouseCaptured() { return mouseCaptured; }

void Input::BindKey(const std::string &action, int key)
{
    if (key >= 0 && key < MOUSE_BUTTON_COUNT)
    {
        actions[action].keys.push_back(key);
        return;
    }

    if (!isValidKey(key)) return;
    actions[action].keys.push_back(key);
}

void Input::BindGamepadButton(const std::string &action, int joystickId, int button)
{
    actions[action].gamepadButtons.push_back({joystickId, button});
}

void Input::UnbindKey(const std::string &action, int key)
{
    auto it = actions.find(action);
    if (it == actions.end()) return;

    auto &keys = it->second.keys;
    keys.erase(std::remove(keys.begin(), keys.end(), key), keys.end());
}

void Input::ClearAction(const std::string &action) { actions.erase(action); }

bool Input::actionBindingDown(const ActionBinding &binding)
{
    for (int key : binding.keys)
    {
        if (key >= 0 && key < MOUSE_BUTTON_COUNT)
        {
            if (IsMouseButtonDownRaw(key)) return true;
        }
        else if (isValidKey(key) && IsKeyDownRaw(key))
        {
            return true;
        }
    }

    for (const auto &gp : binding.gamepadButtons)
    {
        if (IsGamepadButtonDown(gp.first, gp.second)) return true;
    }

    return false;
}

bool Input::actionBindingJustPressed(const ActionBinding &binding)
{
    for (int key : binding.keys)
    {
        if (key >= 0 && key < MOUSE_BUTTON_COUNT)
        {
            if (IsMouseButtonJustPressedRaw(key)) return true;
        }
        else if (isValidKey(key) && IsKeyJustPressedRaw(key))
        {
            return true;
        }
    }

    for (const auto &gp : binding.gamepadButtons)
    {
        if (IsGamepadButtonJustPressed(gp.first, gp.second)) return true;
    }

    return false;
}

bool Input::actionBindingJustReleased(const ActionBinding &binding)
{
    for (int key : binding.keys)
    {
        if (key >= 0 && key < MOUSE_BUTTON_COUNT)
        {
            if (IsMouseButtonJustReleasedRaw(key)) return true;
        }
        else if (isValidKey(key) && IsKeyJustReleasedRaw(key))
        {
            return true;
        }
    }

    for (const auto &gp : binding.gamepadButtons)
    {
        if (IsGamepadButtonJustReleased(gp.first, gp.second)) return true;
    }

    return false;
}

bool Input::actionPassesUIBlock(const ActionBinding &binding)
{
    bool hasKeyboard = false;
    bool hasMouse = false;

    for (int key : binding.keys)
    {
        if (key >= 0 && key < MOUSE_BUTTON_COUNT) hasMouse = true;
        else hasKeyboard = true;
    }

    if (hasKeyboard && uiBlocksKeyboard) return false;
    if (hasMouse && uiBlocksMouse) return false;
    return true;
}

bool Input::IsActionDown(const std::string &action)
{
    if (!isGameplayAllowed()) return false;

    auto it = actions.find(action);
    if (it == actions.end()) return false;
    if (!actionPassesUIBlock(it->second)) return false;

    return actionBindingDown(it->second);
}

bool Input::IsActionJustPressed(const std::string &action)
{
    if (!isGameplayAllowed()) return false;

    auto it = actions.find(action);
    if (it == actions.end()) return false;
    if (!actionPassesUIBlock(it->second)) return false;

    return actionBindingJustPressed(it->second);
}

bool Input::IsActionJustReleased(const std::string &action)
{
    if (!isGameplayAllowed()) return false;

    auto it = actions.find(action);
    if (it == actions.end()) return false;
    if (!actionPassesUIBlock(it->second)) return false;

    return actionBindingJustReleased(it->second);
}

void Input::BindAxisKeys(const std::string &axis, int negativeKey, int positiveKey)
{
    AxisBinding &b = axes[axis];
    b.negativeKey = negativeKey;
    b.positiveKey = positiveKey;
}

void Input::BindAxisGamepad(const std::string &axis, int joystickId, int axisIndex, bool invert, float deadzone)
{
    AxisBinding &b = axes[axis];
    b.joystickId = joystickId;
    b.axisIndex = axisIndex;
    b.invert = invert;
    b.deadzone = deadzone;
}

float Input::GetAxis(const std::string &axis)
{
    if (!isGameplayAllowed()) return 0.0f;

    auto it = axes.find(axis);
    if (it == axes.end()) return 0.0f;

    const AxisBinding &b = it->second;
    float value = 0.0f;

    if (b.negativeKey != GLFW_KEY_UNKNOWN && IsKeyDownRaw(b.negativeKey)) value -= 1.0f;
    if (b.positiveKey != GLFW_KEY_UNKNOWN && IsKeyDownRaw(b.positiveKey)) value += 1.0f;

    if (uiBlocksKeyboard && (b.negativeKey != GLFW_KEY_UNKNOWN || b.positiveKey != GLFW_KEY_UNKNOWN)) value = 0.0f;

    if (b.axisIndex >= 0 && IsGamepadConnected(b.joystickId))
    {
        float gp = GetGamepadAxis(b.joystickId, b.axisIndex, b.deadzone);
        if (b.invert) gp = -gp;
        if (std::fabs(gp) > 0.0f) value = gp;
    }

    return std::clamp(value, -1.0f, 1.0f);
}

bool Input::IsGamepadConnected(int joystickId)
{
    return glfwJoystickPresent(joystickId) == GLFW_TRUE && glfwJoystickIsGamepad(joystickId) == GLFW_TRUE;
}

float Input::GetGamepadAxis(int joystickId, int axisIndex, float deadzone)
{
    if (!IsGamepadConnected(joystickId)) return 0.0f;

    int count = 0;
    const float *axes = glfwGetJoystickAxes(joystickId, &count);
    if (!axes || axisIndex < 0 || axisIndex >= count) return 0.0f;

    float v = axes[axisIndex];
    if (std::fabs(v) < deadzone) return 0.0f;
    return v;
}

bool Input::IsGamepadButtonDown(int joystickId, int button)
{
    if (joystickId != GLFW_JOYSTICK_1 || button < 0 || button >= GAMEPAD_BUTTON_COUNT) return false;
    return gamepadDown[button];
}

bool Input::IsGamepadButtonJustPressed(int joystickId, int button)
{
    if (joystickId != GLFW_JOYSTICK_1 || button < 0 || button >= GAMEPAD_BUTTON_COUNT) return false;
    return gamepadDown[button] && !gamepadDownPrev[button];
}

bool Input::IsGamepadButtonJustReleased(int joystickId, int button)
{
    if (joystickId != GLFW_JOYSTICK_1 || button < 0 || button >= GAMEPAD_BUTTON_COUNT) return false;
    return !gamepadDown[button] && gamepadDownPrev[button];
}

bool Input::ShouldClose() { return closeRequested; }

void Input::ResetCloseRequest() { closeRequested = false; }
