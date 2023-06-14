#pragma once
#include "./Server.hpp"
#include <algorithm>
#include <string>
#include <vector>
#include <utility>
#include <functional>

namespace LinqServer
{
    enum class ClientRequestType
    {
        CONNECT,
        WORD,
        CAST,
        INVALID
    };

    enum class PlayerRole
    {
        COUNTER_SPY,
        SPY
    };

    struct Turn
    {
        int round;
        int player_index;
    };

    struct Player
    {
        int id;
        std::string username;
        PlayerRole role;
        std::vector<std::string> words;
        std::pair<int, int> votes;
    };

    struct PlayerRequest
    {
        ClientRequestType type;
        std::vector<std::string> args;

        PlayerRequest(std::string request);
    };

    class LinqGame
    {
    public:
        std::vector<Player> players;
        std::string spies_word;
        Turn turn;

        LinqGame();
        ~LinqGame();

        void addPlayer(int id, std::string username);
        Player &getPlayerById(int id);
        Player &getPlayerByUsername(std::string username, bool ignore_case = true);
        std::string generateConnectionMessageForPlayer(int id);
        void assignRoles();
        void shufflePlayers();
        std::string generateStartMessage(int id);
        void startGame();
        std::string generatePlayMessage(Turn turn);
        bool nextTurn();
        std::string generateSaidMessage(int id);
        std::string generateVotedMessage(int id);
        PlayerRole countVotes();
        std::vector<int> getWinners(PlayerRole role);
        std::string generateWinMessage();
    };

    class LinqServerThread
    {
    private:
        void onMessage(ServerConnection *server, int client_id, std::string message);
        void onConnect(ServerConnection *server, int client_id);
        void onDisconnect(ServerConnection *server, int client_id);

        ServerConnection *server;
        LinqGame game;

    public:
        LinqServerThread(std::string ip, int port);
        ~LinqServerThread();

        void startThread(ServerStateHandler onStart);
    };
}