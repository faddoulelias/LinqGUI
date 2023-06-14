#include "./headers/LinqComponents.hpp"
#include <iostream>

using namespace LinqComponents;

Flow::Image *LinqComponents::setPageBackground(Flow::Window *window, int page_id, std::string path)
{
    Flow::Image *background = new Flow::Image(window);
    background->setPath(path);
    background->setRelativePosition({0, 0});
    background->setBackground({0, 0, 255, 255});
    background->setRatioDimension({1, 1, 1});
    background->setChildReference(Flow::ReferencePoint::BottomRight);
    background->setParentReference(Flow::ReferencePoint::BottomRight);
    window->addComponent(page_id, background);

    return background;
}

Flow::Image *LinqComponents::createMenuBoard(Flow::Window *window, Flow::Component *parent, std::string title, int page_id)
{
    Flow::Image *spy_board = new Flow::Image(parent);
    spy_board->setPath(LinqComponents::MenuBoardImagePath);
    spy_board->setRelativePosition({0, 0});
    spy_board->setBackground({0, 0, 255, 255});
    spy_board->setDimension({350, 600});
    spy_board->setChildReference(Flow::ReferencePoint::Center);
    spy_board->setParentReference(Flow::ReferencePoint::Center);
    window->addComponent(page_id, spy_board);

    Flow::Text *text = new Flow::Text(spy_board);
    text->setText(title);
    text->loadFont("res/fonts/Old_Standard_TT/OldStandardTT-Regular.ttf", 30);
    text->setRelativePosition({0, 30});
    text->setAutoSize(true);
    text->setBackground({0, 0, 0, 0});
    text->setColor({12, 12, 12, 255});
    text->setChildReference(Flow::ReferencePoint::TopCenter);
    text->setParentReference(Flow::ReferencePoint::TopCenter);
    window->addComponent(page_id, text);

    return spy_board;
}

Flow::Image *LinqComponents::createBoardActionButton(Flow::Window *window, Flow::Component *parent, std::string title, double row_index, int page_id, std::function<void(Flow::Window *, Flow::Component *)> onClick)
{
    std::function<void(Flow::Component *)> onHoverEnter = [](Flow::Component *component)
    { ((Flow::Image *)component)->setPath(LinqComponents::MenuBoardButtonHoverImagePath); };

    std::function<void(Flow::Component *)> onHoverExit = [](Flow::Component *component)
    { ((Flow::Image *)component)->setPath(LinqComponents::MenuBoardButtonImagePath); };

    Flow::Image *create_game_button_frame = new Flow::Image(parent);
    create_game_button_frame->setPath("./res/images/ButtonFrame.png");
    create_game_button_frame->setRelativePosition({0, (int)(25 * row_index)});
    create_game_button_frame->setBackground({0, 0, 255, 255});
    create_game_button_frame->setDimension({300, 50});
    create_game_button_frame->setChildReference(Flow::ReferencePoint::Center);
    create_game_button_frame->setParentReference(Flow::ReferencePoint::Center);
    create_game_button_frame->onClick(onClick);
    create_game_button_frame->onHoverEnter(onHoverEnter);
    create_game_button_frame->onHoverExit(onHoverExit);

    window->addComponent(page_id, create_game_button_frame);

    Flow::Text *create_game_button_text = new Flow::Text(create_game_button_frame);
    create_game_button_text->setText(title);
    create_game_button_text->loadFont("res/fonts/Old_Standard_TT/OldStandardTT-Regular.ttf", 30);
    create_game_button_text->setRelativePosition({0, 0});
    create_game_button_text->setAutoSize(true);
    create_game_button_text->setBackground({0, 0, 0, 0});
    create_game_button_text->setColor({12, 12, 12, 255});
    create_game_button_text->setChildReference(Flow::ReferencePoint::Center);
    create_game_button_text->setParentReference(Flow::ReferencePoint::Center);
    window->addComponent(page_id, create_game_button_text);

    return create_game_button_frame;
}

Flow::Image *LinqComponents::createBoardNavigationButton(Flow::Window *window, Flow::Component *parent, std::string title, double row_index, int page_id, int destination_page_id)
{
    return LinqComponents::createBoardActionButton(window, parent, title, row_index, page_id, [destination_page_id](Flow::Window *window, Flow::Component *component)
                                                   { window->setCurrentPage(destination_page_id); });
}

Flow::Text *LinqComponents::createInputFrame(Flow::Window *window, Flow::Component *parent, std::string title, double row_index, int page_id)
{
    Flow::Image *input_frame = new Flow::Image(parent);
    Flow::Text *input_frame_text = new Flow::Text(input_frame);

    std::function<void(Flow::Component *)> onHoverEnter = [](Flow::Component *component)
    { if (!((Flow::Image *)component)->isFocused()) ((Flow::Image *)component)->setPath(LinqComponents::InputFrameHoverImagePath); };

    std::function<void(Flow::Component *)> onHoverExit = [](Flow::Component *component)
    { if (!((Flow::Image *)component)->isFocused()) ((Flow::Image *)component)->setPath(LinqComponents::InputFrameImagePath); };

    std::function<void(Flow::Component *)> onFocused = [](Flow::Component *component)
    { ((Flow::Image *)component)->setPath(LinqComponents::InputFrameFocusImagePath); };

    std::function<void(Flow::Component *)> onUnfocused = [](Flow::Component *component)
    { ((Flow::Image *)component)->setPath(LinqComponents::InputFrameImagePath); };

    std::function<void(Flow::Window *, Flow::Component *, std::string, Flow::Text * displayer)>
        onWrite = [](Flow::Window *window, Flow::Component *component, std::string text, Flow::Text *displayer)
    {
        if (((Flow::Image *)component)->isFocused())
        {
            std::string output = displayer->getText();
            if (text == "\b")
            {
                if (output.size() > 0)
                    output.pop_back();
            }
            else
            {
                output += text;
            }

            displayer->setText(output);
        }
    };

    // Input frame
    input_frame->setPath(LinqComponents::InputFrameImagePath);
    input_frame->setRelativePosition({0, (int)(25 * row_index)});
    input_frame->setBackground({0, 0, 255, 255});
    input_frame->setDimension({300, 50});
    input_frame->setChildReference(Flow::ReferencePoint::Center);
    input_frame->setParentReference(Flow::ReferencePoint::Center);
    input_frame->onClick([onFocused](Flow::Window *window, Flow::Component *component)
                         { ((Flow::Image *)component)->setFocused(true); onFocused(component); });
    input_frame->onClickOutside([onUnfocused](Flow::Window *window, Flow::Component *component)
                                { ((Flow::Image *)component)->setFocused(false); onUnfocused(component); });
    input_frame->onHoverEnter(onHoverEnter);
    input_frame->onHoverExit(onHoverExit);
    input_frame->onWrite([onWrite, input_frame_text](Flow::Window *window, Flow::Component *component, std::string text)
                         { onWrite(window, component, text, input_frame_text); });

    window->addComponent(page_id, input_frame);

    // Input frame text
    input_frame_text->setText(title);
    input_frame_text->loadFont("res/fonts/Old_Standard_TT/OldStandardTT-Regular.ttf", 30);
    input_frame_text->setRelativePosition({0, 0});
    input_frame_text->setAutoSize(true);
    input_frame_text->setBackground({0, 0, 0, 0});
    input_frame_text->setColor({12, 12, 12, 255});
    input_frame_text->setChildReference(Flow::ReferencePoint::Center);
    input_frame_text->setParentReference(Flow::ReferencePoint::Center);
    window->addComponent(page_id, input_frame_text);

    return input_frame_text;
}

Flow::Text *LinqComponents::createTextLine(Flow::Window *window, Flow::Component *parent, std::string text, std::string font_path, int font_size, double row_index, int page_id)
{
    Flow::Text *text_section = new Flow::Text(parent);
    text_section->setText(text);
    text_section->loadFont(font_path, font_size);
    text_section->setRelativePosition({20, 50 + (int)(25 * row_index)});
    text_section->setAutoSize(true);
    text_section->setBackground({0, 0, 0, 0});
    text_section->setColor({12, 12, 12, 255});
    text_section->setChildReference(Flow::ReferencePoint::TopLeft);
    text_section->setParentReference(Flow::ReferencePoint::TopLeft);
    window->addComponent(page_id, text_section);

    return text_section;
}

Flow::Image *createAvatarSection(Flow::Window *window, Flow::Component *parent, std::string username, Role role, std::string secret_word, int page_id)
{
    Flow::Image *avatar_section = new Flow::Image(parent);
    avatar_section->setPath(LinqComponents::AvatarBoardImagePath);
    avatar_section->setRelativePosition({20, 20});
    avatar_section->setBackground({0, 0, 255, 255});
    avatar_section->setDimension({700, 130});
    avatar_section->setChildReference(Flow::ReferencePoint::TopLeft);
    avatar_section->setParentReference(Flow::ReferencePoint::TopLeft);
    window->addComponent(page_id, avatar_section);

    Flow::Image *avatar = new Flow::Image(avatar_section);
    if (role == Role::Spy)
        avatar->setPath(LinqComponents::SpyAvatarImagePath);
    else
        avatar->setPath(LinqComponents::CounterSpyAvatarImagePath);

    avatar->setRelativePosition({15, 5});
    avatar->setBackground({0, 0, 255, 255});
    avatar->setDimension({100, 100});
    avatar->setChildReference(Flow::ReferencePoint::CenterLeft);
    avatar->setParentReference(Flow::ReferencePoint::CenterLeft);
    window->addComponent(page_id, avatar);

    std::string lines[3];
    lines[0] = "Hello, " + username;
    if (role == Role::Spy)
        lines[1] = "You are a spy";
    else
        lines[1] = "You are a counter-spy";

    if (role == Role::Spy)
        lines[2] = "The secret word to identify the other spy is: '" + secret_word + "'";
    else
        lines[2] = "Your mission is to identify the two spies";

    Flow::Text *line1 = new Flow::Text(avatar_section);
    line1->setText(lines[0]);
    line1->loadFont("res/fonts/Old_Standard_TT/OldStandardTT-Italic.ttf", 23);
    line1->setRelativePosition({120, -28});
    line1->setAutoSize(true);
    line1->setBackground({0, 0, 0, 0});
    line1->setColor({12, 12, 12, 255});
    line1->setChildReference(Flow::ReferencePoint::CenterLeft);
    line1->setParentReference(Flow::ReferencePoint::CenterLeft);
    window->addComponent(page_id, line1);

    Flow::Text *line2 = new Flow::Text(avatar_section);
    line2->setText(lines[1]);
    line2->loadFont("res/fonts/Old_Standard_TT/OldStandardTT-Italic.ttf", 23);
    line2->setRelativePosition({120, 0});
    line2->setAutoSize(true);
    line2->setBackground({0, 0, 0, 0});
    line2->setColor({12, 12, 12, 255});
    line2->setChildReference(Flow::ReferencePoint::CenterLeft);
    line2->setParentReference(Flow::ReferencePoint::CenterLeft);
    window->addComponent(page_id, line2);

    Flow::Text *line3 = new Flow::Text(avatar_section);
    line3->setText(lines[2]);
    line3->loadFont("res/fonts/Old_Standard_TT/OldStandardTT-Italic.ttf", 23);
    line3->setRelativePosition({120, 28});
    line3->setAutoSize(true);
    line3->setBackground({0, 0, 0, 0});
    line3->setColor({12, 12, 12, 255});
    line3->setChildReference(Flow::ReferencePoint::CenterLeft);
    line3->setParentReference(Flow::ReferencePoint::CenterLeft);
    window->addComponent(page_id, line3);

    return avatar_section;
}

Flow::Image *createCentralGameBoard(Flow::Window *window, Flow::Component *parent, int page_id)
{
    const double ratio = 0.786088257;
    const double height = 600;

    Flow::Image *game_illustration = new Flow::Image(parent);
    game_illustration->setPath(LinqComponents::MorseIlustrationImagePath);
    game_illustration->setRelativePosition({30, 160});
    game_illustration->setBackground({0, 0, 255, 255});
    game_illustration->setDimension({(int)(height * ratio), (int)height});
    game_illustration->setChildReference(Flow::ReferencePoint::TopLeft);
    game_illustration->setParentReference(Flow::ReferencePoint::TopLeft);
    window->addComponent(page_id, game_illustration);

    Flow::Image *game_board = new Flow::Image(parent);
    game_board->setPath(LinqComponents::GameBoardImagePath);
    game_board->setRelativePosition({(int)(30 + height * ratio + 30), 160});
    game_board->setBackground({0, 0, 255, 255});
    game_board->setDimension({800, (int)height});
    game_board->setChildReference(Flow::ReferencePoint::TopLeft);
    game_board->setParentReference(Flow::ReferencePoint::TopLeft);
    window->addComponent(page_id, game_board);

    return game_board;
}

Flow::Image *LinqComponents::createGameBoard(Flow::Window *window, Flow::Component *parent, std::string username, Role role, std::string secret_word, int page_id)
{
    Flow::Frame *centered_section = new Flow::Frame(parent);
    centered_section->setBackground({0, 0, 0, 0});
    centered_section->setDimension({1360, 800});
    centered_section->setChildReference(Flow::ReferencePoint::Center);
    centered_section->setParentReference(Flow::ReferencePoint::Center);
    window->addComponent(page_id, centered_section);

    Flow::Image *game_spy_avatar_frame = createAvatarSection(window, centered_section, username, role, secret_word, page_id);
    Flow::Image *game_board = createCentralGameBoard(window, centered_section, page_id);
    return game_board;
}