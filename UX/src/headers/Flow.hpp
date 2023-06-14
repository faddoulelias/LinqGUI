#pragma once
#include <vector>
#include <string>
#include <map>

namespace Flow
{
    void InitializeGUI();

    class ObjectComponent;

    struct ObjectComponentPagePair
    {
        int id;
        ObjectComponent *component;
    };

    enum class ReferencePoint
    {
        Absolute,
        TopLeft,
        TopCenter,
        TopRight,
        CenterLeft,
        Center,
        CenterRight,
        BottomLeft,
        BottomCenter,
        BottomRight,
    };

    struct Dimension
    {
        int width;
        int height;
    };

    struct Position
    {
        int x;
        int y;
    };

    struct Color
    {
        int r;
        int g;
        int b;
        int a;
    };

    class Component
    {
    protected:
        Position position;
        Dimension dimension;
        Color background;

    public:
        Position getPosition();
        void setPosition(Position position);

        Dimension getDimension();
        void setDimension(Dimension dimension);

        Color getBackground();
        void setBackground(Color color);
    };

    class Window : public Component
    {
    private:
        std::string title;

        bool resizable;
        bool isClosed;

        void *window;
        void *renderer;

        std::vector<std::pair<int, ObjectComponent *>> children;
        int current_page;

        void handleEvent(void *event);
        void render();

    public:
        Window();
        ~Window();

        bool isResizable();
        void setResizable(bool resizable);

        void close();
        bool isOpen();

        void setTitle(std::string title);
        std::string getTitle();

        void *getRenderer();

        void addComponent(int page_id, ObjectComponent *component);
        void removeComponent(int id);

        void setCurrentPage(int page_id);
        int getCurrentPage();

        void mainLoop();
    };
}