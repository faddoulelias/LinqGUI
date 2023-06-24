#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>

#include "headers/Flow.hpp"
#include "headers/FlowComponents.hpp"

using namespace std;
using namespace Flow;

void Flow::InitializeGUI()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        throw runtime_error("error initializing SDL: " + string(SDL_GetError()));
    }

    if (TTF_Init() != 0)
    {
        throw runtime_error("error initializing TTF: " + string(TTF_GetError()));
    }
}

/**
 * Position
 */
Position Component::getPosition()
{
    return this->position;
}

void Component::setPosition(Position position)
{
    this->position = position;
}

Dimension Component::getDimension()
{
    return this->dimension;
}

void Component::setDimension(Dimension dimension)
{
    this->dimension = dimension;
}

Color Component::getBackground()
{
    return this->background;
}

void Component::setBackground(Color color)
{
    this->background = color;
}

/**
 * Window
 */

Window::Window()
{
    this->position.x = 0;
    this->position.y = 0;
    this->dimension.width = 1000;
    this->dimension.height = 1000;
    this->background = {255, 255, 255, 255};
    this->title = "Flow";
    this->resizable = false;
    this->isClosed = false;
    this->window = nullptr;
    this->renderer = nullptr;
    this->on_next_render = nullptr;
}

Window::~Window() = default;

bool Window::isResizable()
{
    return this->resizable;
}

void Window::setResizable(bool resizable)
{
    this->resizable = resizable;
}

void Window::close()
{
    this->isClosed = true;
}

bool Window::isOpen()
{
    return !this->isClosed;
}

void Window::setTitle(string title)
{
    this->title = title;
}

string Window::getTitle()
{
    return this->title;
}

void *Window::getRenderer()
{
    return this->renderer;
}

int Window::addComponent(int page_id, ObjectComponent *component)
{
    int location = this->children.size();
    this->children.push_back(make_pair(page_id, component));

    return location;
}

void Window::removeComponent(int id)
{
    auto it = std::remove_if(this->children.begin(), this->children.end(), [&, id](std::pair<int, Flow::ObjectComponent *> cmp)
                             { return cmp.second->getId() == id; });

    this->children.erase(it, this->children.end());
}

void Window::removeComponentsByLabel(std::string label)
{
    auto it = std::remove_if(this->children.begin(), this->children.end(), [&, label](std::pair<int, Flow::ObjectComponent *> cmp)
                             { return cmp.second->getLabel() == label; });

    this->children.erase(it, this->children.end());
}

void Window::handleEvent(void *event)
{
    SDL_Event *current_event = (SDL_Event *)event;
    switch (current_event->type)
    {
    case SDL_QUIT:
        this->isClosed = true;
        break;

    case SDL_WINDOWEVENT:
        switch (current_event->window.event)
        {
        case SDL_WINDOWEVENT_RESIZED:
            this->dimension.width = current_event->window.data1;
            this->dimension.height = current_event->window.data2;
            break;
        }
        break;
    case SDL_MOUSEBUTTONDOWN:
        for (auto it = this->children.begin(); it != this->children.end(); ++it)
        {
            if (it->first != this->current_page)
            {
                it->second->forceUnhover();
                continue;
            }

            if (it->second->isHovered())
            {
                it->second->handleOnClick(this);
            }
        }
        for (auto it = this->children.begin(); it != this->children.end(); ++it)
        {
            if (!it->second->isHovered())
            {
                it->second->handleOnClickOutside(this);
            }
        }
        break;

    // case we write a character or backspace
    case SDL_TEXTINPUT:
        for (auto it = this->children.begin(); it != this->children.end(); ++it)
        {
            if (it->first != this->current_page)
            {
                it->second->forceUnhover();
                continue;
            }

            it->second->handleOnWrite(this, current_event->text.text);
        }
        break;
    // handle backspace
    case SDL_KEYDOWN:
        if (current_event->key.keysym.sym == SDLK_BACKSPACE)
        {
            for (auto it = this->children.begin(); it != this->children.end(); ++it)
            {
                if (it->first != this->current_page)
                {
                    it->second->forceUnhover();
                    continue;
                }

                it->second->handleOnWrite(this, "\b");
            }
        }
        break;
    }
}

void Window::render()
{
    SDL_SetRenderDrawColor((SDL_Renderer *)this->renderer, this->background.r, this->background.g, this->background.b, this->background.a);
    SDL_RenderClear((SDL_Renderer *)this->renderer);

    bool clickable_component_hovered = false;

    for (auto it = this->children.begin(); it != this->children.end(); ++it)
    {
        if (it->first != this->current_page)
            continue;

        it->second->render(this);
        if (it->second->isHovered() && it->second->isClickable())
        {
            clickable_component_hovered = true;
        }
    }

    if (clickable_component_hovered)
    {
        SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND));
    }
    else
    {
        SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
    }

    SDL_RenderPresent((SDL_Renderer *)this->renderer);
}

void Window::mainLoop()
{
    this->window = SDL_CreateWindow(this->title.c_str(),
                                    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                    this->dimension.width, this->dimension.height,
                                    SDL_WINDOW_SHOWN | (this->resizable ? SDL_WINDOW_RESIZABLE : 0));

    renderer = SDL_CreateRenderer((SDL_Window *)this->window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawBlendMode((SDL_Renderer *)this->renderer, SDL_BLENDMODE_BLEND);

    if (!this->window)
    {
        throw runtime_error("error creating window: " + string(SDL_GetError()));
    }

    while (!this->isClosed)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (this->on_next_render != nullptr)
            {
                this->on_next_render();
                this->on_next_render = nullptr;
            }
            this->render();
            this->handleEvent(&event);
        }
    }

    SDL_DestroyRenderer((SDL_Renderer *)this->renderer);
    SDL_DestroyWindow((SDL_Window *)this->window);
    SDL_Quit();
}

void Window::onNextRender(std::function<void()> on_next_render)
{
    this->on_next_render = on_next_render;
}

void Window::setCurrentPage(int page_id)
{
    this->current_page = page_id;

    for (auto it = this->children.begin(); it != this->children.end(); ++it)
    {
        if (it->first != this->current_page)
        {
            it->second->forceUnhover();
            continue;
        }
    }
}

int Window::getCurrentPage()
{
    return this->current_page;
}