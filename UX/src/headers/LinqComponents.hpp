#pragma once
#include "Flow.hpp"
#include "FlowComponents.hpp"
#include <string>

namespace LinqComponents
{
    enum class Role
    {
        CounterSpy,
        Spy,
    };

    const std::string OldStandardTT = "res/fonts/Old_Standard_TT/OldStandardTT-Regular.ttf";
    const std::string OldStandardTTBold = "res/fonts/Old_Standard_TT/OldStandardTT-Bold.ttf";
    const std::string OldStandardTTItalic = "res/fonts/Old_Standard_TT/OldStandardTT-Italic.ttf";

    const std::string MainBackgroundImagePath = "res/images/Background.png";
    const std::string MenuBoardImagePath = "./res/images/SpyBoard.png";
    const std::string MenuBoardButtonImagePath = "./res/images/ButtonFrame.png";
    const std::string MenuBoardButtonHoverImagePath = "./res/images/ButtonFrameHover.png";
    const std::string MenuBoardButtonRedImagePath = "./res/images/ButtonRedFrame.png";
    const std::string InputFrameImagePath = "./res/images/InputFrame.png";
    const std::string InputFrameHoverImagePath = "./res/images/InputFrameHover.png";
    const std::string InputFrameFocusImagePath = "./res/images/InputFrameFocus.png";

    const std::string GameBackgroundImagePath = "./res/images/GameBackground.png";
    const std::string GameBoardImagePath = "./res/images/GameBoard.png";
    const std::string AvatarBoardImagePath = "./res/images/AvatarBoard.png";
    const std::string SpyAvatarImagePath = "./res/images/SpyAvatar.png";
    const std::string CounterSpyAvatarImagePath = "./res/images/CounterSpyAvatar.png";
    const std::string MorseIlustrationImagePath = "./res/images/MorseIlustration.png";

    Flow::Image *setPageBackground(Flow::Window *window, int page_id, std::string path);
    Flow::Image *createMenuBoard(Flow::Window *window, Flow::Component *parent, std::string title, int page_id);
    Flow::Image *createBoardNavigationButton(Flow::Window *window, Flow::Component *parent, std::string title, double row_index, int page_id, int destination_page_id);
    Flow::Image *createBoardActionButton(Flow::Window *window, Flow::Component *parent, std::string title, double row_index, int page_id, std::function<void(Flow::Window *, Flow::Component *)> onClick);
    Flow::Text *createInputFrame(Flow::Window *window, Flow::Component *parent, std::string title, double row_index, int page_id);
    Flow::Text *createTextLine(Flow::Window *window, Flow::Component *parent, std::string text, std::string font_path, int font_size, double row_index, int page_id);

    Flow::Image *createGameBoard(Flow::Window *window, Flow::Component *parent, std::string username, Role role, std::string secret_word, int page_id);

    std::string createGameDataSection(Flow::Window *window, Flow::Component *parent, std::map<std::string, std::string> conv,
                                      std::string current_player, std::function<void(std::string)> onSend, bool is_my_turn, int page_id);

    std::string createVoteDataSection(Flow::Window *window, Flow::Component *parent, int page_id, bool is_your_turn,
                                      std::string player_voting, std::vector<std::string> players,
                                      std::function<void(std::pair<std::string, std::string>)> voteHandler,
                                      std::pair<std::string, std::string> &selected,
                                      LinqComponents::Role role);
}
