#pragma once

#include <cstdint>
#include <functional>

namespace exd::core {

// ────────────────────────────────────────────────────
//  WindowState — abstract input/display interface
// ────────────────────────────────────────────────────
//  Moved from extropian-app to break the dependency
//  chain. Both desktop and web platforms implement
//  this interface:
//    Desktop: exd::app::Window (SDL3-backed)
//    Web:     browser event handler
//
//  Provides cursor, keyboard, mouse, scroll, and
//  gamepad state in a platform-agnostic format.

enum class InputMode {
    FPS = 0,  // cursor hidden & captured
    UI  = 1   // cursor visible, UI-style interaction
};

enum class CursorMode {
    Normal   = 0,  ///< Visible, not captured
    Hidden   = 1,  ///< Invisible but free to move
    Captured = 2   ///< Hidden + locked to window (for FPS camera)
};

/// Platform-agnostic window lifecycle events.
struct WindowEvents {
    /// Called when the framebuffer size changes (pixels).
    std::function<void(int width, int height)> on_resize;

    /// Called when the window gains or loses keyboard focus.
    std::function<void(bool focused)> on_focus;

    /// Called when the window is minimized or restored.
    std::function<void(bool minimized)> on_minimize;

    /// Called when the user requests window close (X button, Alt+F4).
    std::function<void()> on_close;
};

struct WindowState {
    virtual ~WindowState() = default;

    /// Retrieve window dimensions (pixels) and aspect ratio.
    virtual void get_dimensions(int& w, int& h, float& aspect) const = 0;

    /// Was a key pressed this frame? (scancode, not keycode)
    [[nodiscard]] virtual bool was_key_pressed(int scancode) const = 0;

    /// Was a key released this frame?
    [[nodiscard]] virtual bool was_key_released(int scancode) const = 0;

    /// Reset accumulated mouse delta after consuming it.
    virtual void reset_mouse_delta() = 0;

    /// Is a mouse button currently held?
    [[nodiscard]] virtual bool mouse_button_down(int button) const = 0;

    // ── Input state (updated each frame by the platform layer) ──

    InputMode   input_mode      = InputMode::FPS;
    const bool* keyboard_state  = nullptr;
    float       mouse_rel_x     = 0.0f;
    float       mouse_rel_y     = 0.0f;
    float       scroll_x        = 0.0f;
    float       scroll_y        = 0.0f;
    bool        grid_visible    = true;
    bool        wireframe       = false;

    float       gamepad_left_x          = 0.0f;
    float       gamepad_left_y          = 0.0f;
    float       gamepad_right_x         = 0.0f;
    float       gamepad_right_y         = 0.0f;
    float       gamepad_left_trigger    = 0.0f;
    float       gamepad_right_trigger   = 0.0f;
    uint16_t    gamepad_buttons         = 0;
};

} // namespace exd::core
