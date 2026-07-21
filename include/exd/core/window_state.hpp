#pragma once

namespace exd::core {

/// Input mode enum — canonical definition lives in core so both
/// the renderer and the application framework can depend on it
/// without introducing a circular dependency.
enum class InputMode {
    FPS = 0,  ///< Relative mouse, camera moves with WASD + Mouse
    UI  = 1   ///< Absolute mouse, UI captures input, camera frozen
};

/// Minimal window-state interface used by the renderer.
///
/// Implemented by `exd::app::Window` to provide the handful of
/// dimension / input / toggle values that the rendering systems
/// need each frame.  The interface is intentionally narrow so that
/// the renderer never needs to include SDL3 or application-layer
/// headers directly.
struct WindowState {
    virtual ~WindowState() = default;

    /// Retrieve the current framebuffer dimensions and aspect ratio.
    virtual void get_dimensions(int& w, int& h, float& aspect) const = 0;

    /// True on the frame that the key with the given platform
    /// scancode was released.  (Scancodes are SDL_SCANCODE_* on
    /// desktop platforms.)
    virtual bool was_key_released(int scancode) const = 0;

    // ── Mutable state (read / written by both app & renderer) ──

    InputMode   input_mode      = InputMode::FPS;
    const bool* keyboard_state  = nullptr;   ///< SDL_GetKeyboardState view
    float       mouse_rel_x     = 0.0f;      ///< accumulated relative X
    float       mouse_rel_y     = 0.0f;      ///< accumulated relative Y
    bool        grid_visible    = true;      ///< toggled by G key
    bool        wireframe       = false;     ///< toggled by X key
};

} // namespace exd::core
