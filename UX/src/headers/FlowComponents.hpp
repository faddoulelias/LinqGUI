#pragma once
#include <functional>
#include "Flow.hpp"

namespace Flow
{
    typedef std::function<void(Window *)> RenderFunction;
    typedef std::function<void(Window *, Component *)> ClickEventFunction;
    typedef std::function<void(Component *)> HoverFunction;
    typedef std::function<void(Window *, Component *, std::string)> WriteFunction;

    struct RatioDimension
    {
        bool is_ratio;
        float width;
        float height;
    };

    class ObjectComponent : public Component
    {
    protected:
        Component *parent;
        Position relative_position;
        RatioDimension ratio_dimension;
        ReferencePoint parent_reference;
        ReferencePoint child_reference;
        ClickEventFunction on_click;
        ClickEventFunction on_click_outside;
        HoverFunction on_hover_enter;
        HoverFunction on_hover_exit;
        WriteFunction on_write;
        bool hovered;
        bool focused;
        int id;

    public:
        ObjectComponent(Component *parent);
        ~ObjectComponent() = default;

        virtual void render(Window *window) = 0;

        int getId();
        void setId(int id);

        Component *getParent();
        void setParent(Component *parent);

        Position getRelativePosition();
        void setRelativePosition(Position relative_position);

        ReferencePoint getParentReference();
        void setParentReference(ReferencePoint parent_reference);

        ReferencePoint getChildReference();
        void setChildReference(ReferencePoint child_reference);

        RatioDimension getRatioDimension();
        void setRatioDimension(RatioDimension relative_dimension);

        bool isHovered();
        void forceUnhover();

        void onHoverEnter(HoverFunction on_hover_enter);
        void onHoverExit(HoverFunction on_hover_exit);

        bool isClickable();

        void onClick(ClickEventFunction on_click);
        void handleOnClick(Window *window);

        void onClickOutside(ClickEventFunction on_click_outside);
        void handleOnClickOutside(Window *window);

        void onWrite(WriteFunction on_write);
        void handleOnWrite(Window *window, std::string characters);

        void setFocused(bool focused);
        bool isFocused();
    };

    class Frame : public ObjectComponent
    {
    public:
        Frame(Component *parent);
        ~Frame() = default;
        void render(Window *window) override;
    };

    class Text : public ObjectComponent
    {
    private:
        std::string text;
        int font_size;
        bool auto_size;
        Color color;
        std::string path;
        void *font;

    public:
        Text(Component *parent);
        ~Text() = default;

        std::string getText();
        void setText(std::string text);

        Color getColor();
        void setColor(Color color);

        bool isAutoSize();
        void setAutoSize(bool auto_size);

        void loadFont(std::string path, int font_size);

        void render(Window *window) override;
    };

    class Image : public ObjectComponent
    {
    private:
        std::string path;
        void *image;

    public:
        Image(Component *parent);
        ~Image() = default;

        std::string getPath();
        void setPath(std::string path);

        void render(Window *window) override;
    };
};