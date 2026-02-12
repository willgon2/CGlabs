#pragma once

#include "framework.h"
#include "image.h"

class Button
{
public:
    enum Type {
        BTN_NONE = 0,
        BTN_LINE,
        BTN_RECT,
        BTN_TRIANGLE,
        BTN_PENCIL,
        BTN_ERASER,
        BTN_COLOR_BLACK,
        BTN_COLOR_WHITE,
        BTN_COLOR_RED,
        BTN_COLOR_GREEN,
        BTN_COLOR_BLUE,
        BTN_COLOR_YELLOW
    };

    Image* image = nullptr;
    Vector2 position;
    int width = 0;
    int height = 0;
    Type type = BTN_NONE;

    Button() = default;

    Button(Image* img, const Vector2& pos, Type t)
    {
        image = img;
        position = pos;
        type = t;

        if (image)
        {
            width = (int)image->width;
            height = (int)image->height;
        }
    }

    bool IsMouseInside(Vector2 mousePosition) const
    {
        return mousePosition.x >= position.x &&
            mousePosition.x < (position.x + width) &&
            mousePosition.y >= position.y &&
            mousePosition.y < (position.y + height);
    }

    void Render(Image& framebuffer) const
    {
        if (!image) return;
        framebuffer.DrawImage(*image, (int)position.x, (int)position.y);
    }
};