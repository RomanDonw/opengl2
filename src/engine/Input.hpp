#ifndef INPUT_HPP
#define INPUT_HPP

#include "external/glm.hpp"
#include "external/opengl.hpp"

#include <string>
#include <unordered_map>
#include <vector>

enum class InputContext
{
    Gameplay,
    UI
};

class Input final
{
    private:
        Input() = delete;

        static constexpr int KEY_COUNT = 512;
        static constexpr int MOUSE_BUTTON_COUNT = 16;
        static constexpr int GAMEPAD_BUTTON_COUNT = 32;
        static constexpr float DEFAULT_DEADZONE = 0.15f;

        struct ActionBinding
        {
            std::vector<int> keys;
            std::vector<std::pair<int, int>> gamepadButtons;
        };

        struct AxisBinding
        {
            int negativeKey = GLFW_KEY_UNKNOWN;
            int positiveKey = GLFW_KEY_UNKNOWN;
            int joystickId = GLFW_JOYSTICK_1;
            int axisIndex = -1;
            bool invert = false;
            float deadzone = DEFAULT_DEADZONE;
        };

        static inline GLFWwindow *window = nullptr;

        static inline bool keysDown[KEY_COUNT]{};
        static inline bool keysDownPrev[KEY_COUNT]{};
        static inline bool mouseDown[MOUSE_BUTTON_COUNT]{};
        static inline bool mouseDownPrev[MOUSE_BUTTON_COUNT]{};
        static inline bool gamepadDown[GAMEPAD_BUTTON_COUNT]{};
        static inline bool gamepadDownPrev[GAMEPAD_BUTTON_COUNT]{};

        static inline double mouseX = 0;
        static inline double mouseY = 0;
        static inline double lastMouseX = 0;
        static inline double lastMouseY = 0;
        static inline glm::vec2 mouseLookDelta = glm::vec2(0.0f);
        static inline glm::vec2 scrollDelta = glm::vec2(0.0f);
        static inline glm::vec2 mouseSensitivity = glm::vec2(0.1f);

        static inline int captureButton = GLFW_MOUSE_BUTTON_RIGHT;
        static inline bool captureToggle = false;
        static inline bool mouseCaptured = false;
        static inline bool captureToggleArmed = false;
        static inline bool closeRequested = false;

        static inline bool uiBlocksKeyboard = false;
        static inline bool uiBlocksMouse = false;
        static inline std::vector<InputContext> contextStack = {InputContext::Gameplay};

        static inline std::unordered_map<std::string, ActionBinding> actions;
        static inline std::unordered_map<std::string, AxisBinding> axes;

        static void keyCallback(GLFWwindow *w, int key, int scancode, int action, int mods);
        static void mouseButtonCallback(GLFWwindow *w, int button, int action, int mods);
        static void cursorPosCallback(GLFWwindow *w, double x, double y);
        static void scrollCallback(GLFWwindow *w, double xoff, double yoff);
        static void charCallback(GLFWwindow *w, unsigned int codepoint);

        static void updateKeyboardState();
        static void updateMouseState();
        static void updateGamepadState();
        static void updateMouseCapture();
        static void applyMouseCapture(bool captured);

        static bool isValidKey(int keycode);
        static bool isValidMouseButton(int button);
        static bool isGameplayAllowed();
        static bool isKeyboardAllowed();
        static bool isMouseAllowed();

        static bool actionBindingDown(const ActionBinding &binding);
        static bool actionBindingJustPressed(const ActionBinding &binding);
        static bool actionBindingJustReleased(const ActionBinding &binding);
        static bool actionPassesUIBlock(const ActionBinding &binding);

    public:
        static void Init(GLFWwindow *w);
        static void Shutdown();
        static void SetupDefaultBindings();

        static void Poll();
        static void EndFrame();
        static void UpdateMouseLook();

        static void SetUIBlocking(bool keyboard, bool mouse);
        static void PushContext(InputContext context);
        static void PopContext();
        static InputContext GetContext();

        static bool HasFocus();

        static bool IsKeyDownRaw(int keycode);
        static bool IsKeyJustPressedRaw(int keycode);
        static bool IsKeyJustReleasedRaw(int keycode);

        static bool IsKeyDown(int keycode);
        static bool IsKeyPressed(int keycode);
        static bool IsKeyJustPressed(int keycode);
        static bool IsKeyJustReleased(int keycode);

        static bool IsMouseButtonDownRaw(int button);
        static bool IsMouseButtonJustPressedRaw(int button);
        static bool IsMouseButtonJustReleasedRaw(int button);

        static bool IsMouseButtonDown(int button);
        static bool IsMouseButtonPressed(int button);
        static bool IsMouseButtonJustPressed(int button);
        static bool IsMouseButtonJustReleased(int button);

        static glm::dvec2 GetMousePosition();
        static glm::vec2 GetMousePositionNormalized();
        static glm::vec2 GetMouseLookDelta();

        static glm::vec2 GetScrollDelta();
        static void ClearScrollDelta();

        static void SetMouseCaptureButton(int button);
        static void SetMouseCaptureToggle(bool toggle);
        static void SetMouseSensitivity(glm::vec2 sensitivity);
        static void SetMouseCaptured(bool captured);
        static bool IsMouseCaptured();

        static void BindKey(const std::string &action, int key);
        static void BindGamepadButton(const std::string &action, int joystickId, int button);
        static void UnbindKey(const std::string &action, int key);
        static void ClearAction(const std::string &action);

        static bool IsActionDown(const std::string &action);
        static bool IsActionJustPressed(const std::string &action);
        static bool IsActionJustReleased(const std::string &action);

        static void BindAxisKeys(const std::string &axis, int negativeKey, int positiveKey);
        static void BindAxisGamepad(const std::string &axis, int joystickId, int axisIndex, bool invert = false, float deadzone = DEFAULT_DEADZONE);
        static float GetAxis(const std::string &axis);

        static bool IsGamepadConnected(int joystickId = GLFW_JOYSTICK_1);
        static float GetGamepadAxis(int joystickId, int axisIndex, float deadzone = DEFAULT_DEADZONE);
        static bool IsGamepadButtonDown(int joystickId, int button);
        static bool IsGamepadButtonJustPressed(int joystickId, int button);
        static bool IsGamepadButtonJustReleased(int joystickId, int button);

        static bool ShouldClose();
        static void ResetCloseRequest();
};

#endif
