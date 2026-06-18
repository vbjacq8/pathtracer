#pragma once

enum class ViewEventType {
    None,
    Quit,
    MouseButtonDown,
    MouseButtonUp,
    MouseMotion,
    MouseWheel,
    KeyDown,
};

/**
 * \brief Platform-neutral input event consumed by RenderController.
 */
struct ViewEvent {
    ViewEventType type = ViewEventType::None;
    int button = 0;
    int x = 0;
    int y = 0;
    int xrel = 0;
    int yrel = 0;
    int key = 0;
    float wheelY = 0.0f;
};
