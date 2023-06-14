#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <utility>
#include <ctime>
#include <map>
#include <thread>

#include "./headers/LinqServer.hpp"

using namespace LinqServer;

std::vector<std::string> words = {
    "Boule",
    "Bouquet",
    "Bourgeon",
    "Bouton",
    "Brindille",
    "Buisson",
    "Champignon",
    "Feuille",
    "Fleur",
    "Fruit",
    "Graine",
    "Herbe",
    "Liane",
    "Mousse",
    "Pétale",
    "Pollen",
    "Racine",
    "Rose",
    "Sève",
    "Tige",
    "Tronc",
    "Végétal",
    "Écorce",
    "Épine",
    "Étamine",
    "Écorce",
    "Épine",
};

PlayerRequest::PlayerRequest(std::string request)
{
    std::istringstream str_stream(request);
    std::string word;

    str_stream >> word;
    if (word == "CONNECT")
        type = ClientRequestType::CONNECT;
    else if (word == "WORD")
        type = ClientRequestType::WORD;
    else if (word == "CAST")
        type = ClientRequestType::CAST;
    else
        type = ClientRequestType::INVALID;

    while (str_stream >> word)
        args.push_back(word);
}

LinqGame::LinqGame()
{
    srand(time(NULL));
    this->players = std::vector<Player>();
    this->turn = Turn{0, 0};
}

LinqGame::~LinqGame() = default;

void LinqGame::addPlayer(int id, std::string username)
{
    Player player;
    player.id = id;
    player.username = username;
    player.role = PlayerRole::COUNTER_SPY;
    this->players.push_back(player);
}

std::string LinqGame::generateConnectionMessageForPlayer(int id)
{
    std::string response = "CONNECTED ";
    response += std::to_string(id);
    response += " ";

    for (const Player &player : this->players)
    {
        response += player.username;
        response += " ";
    }

    for (size_t i = this->players.size(); i < 5; i++)
    {
        response += "-";
        response += " ";
    }

    return response;
}

std::pair<int, int> pickTwoRandomPlayers()
{
    int first_player = rand() % 5;
    int second_player = rand() % 5;

    while (first_player == second_player)
        second_player = rand() % 5;

    return std::make_pair(first_player, second_player);
}

void LinqGame::shufflePlayers()
{
    std::random_shuffle(this->players.begin(), this->players.end());
}

void LinqGame::assignRoles()
{
    std::pair<int, int> spy_players = pickTwoRandomPlayers();
    this->players[spy_players.first].role = PlayerRole::SPY;
    this->players[spy_players.second].role = PlayerRole::SPY;

    this->spies_word = words[rand() % words.size()];
}

std::string LinqGame::generateStartMessage(int id)
{
    std::string response = "START ";
    response += std::to_string(id);
    response += " ";

    if (this->getPlayerById(id).role == PlayerRole::SPY)
    {
        response += "1 ";
        response += this->spies_word;
        response += " ";
    }
    else
    {
        response += "0 - ";
    }

    for (const Player &player : this->players)
    {
        response += player.username;
        response += " ";
    }

    return response;
}

void LinqGame::startGame()
{
    this->turn = Turn{1, 0};
}

std::string LinqGame::generatePlayMessage(Turn turn)
{
    std::string response;
    if (turn.round < 3)
        response = "PLAY ";
    else
        response = "VOTE ";

    response += std::to_string(this->players[turn.player_index].id);
    response += " ";
    response += this->players[turn.player_index].username;

    return response;
}

bool LinqGame::nextTurn()
{
    if (this->turn.player_index == 4)
    {
        this->turn.player_index = 0;
        this->turn.round++;
    }
    else
    {
        this->turn.player_index++;
    }

    if (this->turn.round == 4)
        return 1;

    return 0;
};

Player &LinqGame::getPlayerById(int id)
{
    for (Player &player : this->players)
    {
        if (player.id == id)
            return player;
    }

    throw std::runtime_error("Player not found");
}

Player &LinqGame::getPlayerByUsername(std::string username, bool ignore_case)
{
    std::string username_to_find = username;
    if (ignore_case)
        std::transform(username.begin(), username.end(), username_to_find.begin(), ::tolower);

    for (Player &player : this->players)
    {
        std::string player_username = player.username;
        if (ignore_case)
            std::transform(player.username.begin(), player.username.end(), player_username.begin(), ::tolower);

        if (player_username == username_to_find)
            return player;
    }

    throw std::runtime_error("Player not found");
}

std::string LinqGame::generateSaidMessage(int id)
{
    std::string response = "SAID ";
    response += this->getPlayerById(id).username;
    response += " ";
    response += this->getPlayerById(id).words[this->turn.round - 1];

    return response;
}

std::string LinqGame::generateVotedMessage(int id)
{
    std::string response = "VOTED ";
    response += this->getPlayerById(id).username;
    response += " ";

    response += this->getPlayerById(this->getPlayerById(id).votes.first).username;
    response += " ";
    response += this->getPlayerById(this->getPlayerById(id).votes.second).username;

    return response;
}

PlayerRole LinqGame::countVotes()
{
    std::map<int, int> votes;
    for (Player &player : this->players)
    {
        votes[player.votes.first]++;
        votes[player.votes.second]++;
    }

    int first_spy_id = -1;
    int second_spy_id = -1;

    for (Player &player : this->players)
    {
        if (player.role == PlayerRole::SPY)
        {
            if (first_spy_id == -1)
                first_spy_id = player.id;
            else
                second_spy_id = player.id;
        }
    }

    bool isSpyOneSmart = this->getPlayerById(first_spy_id).votes.first == second_spy_id || this->getPlayerById(first_spy_id).votes.second == second_spy_id;
    bool isSpyTwoSmart = this->getPlayerById(second_spy_id).votes.first == first_spy_id || this->getPlayerById(second_spy_id).votes.second == first_spy_id;

    if (isSpyOneSmart && isSpyTwoSmart)
        return PlayerRole::SPY;

    int max_votes_1 = -1;
    int max_votes_2 = -1;
    int max_voted_player_1 = -1;
    int max_voted_player_2 = -1;

    for (auto &vote : votes)
    {
        if (vote.second > max_votes_1)
        {
            max_votes_2 = max_votes_1;
            max_voted_player_2 = max_voted_player_1;

            max_votes_1 = vote.second;
            max_voted_player_1 = vote.first;
        }
        else if (vote.second > max_votes_2)
        {
            max_votes_2 = vote.second;
            max_voted_player_2 = vote.first;
        }
    }

    if (max_votes_1 > 2 && max_votes_2 > 2)
    {
        if (max_voted_player_1 == first_spy_id || max_voted_player_1 == second_spy_id)
            return PlayerRole::SPY;
        else if (max_voted_player_2 == first_spy_id || max_voted_player_2 == second_spy_id)
            return PlayerRole::SPY;
    }

    return PlayerRole::COUNTER_SPY;
}

std::vector<int> LinqGame::getWinners(PlayerRole role)
{
    std::vector<int> winners;
    for (Player &player : this->players)
    {
        if (player.role == role)
            winners.push_back(player.id);
    }

    return winners;
}

std::string LinqGame::generateWinMessage()
{
    std::string response = "WIN ";
    PlayerRole winner_role = this->countVotes();

    response += std::to_string((int)winner_role);
    response += " ";

    std::vector<int> winners = this->getWinners(winner_role);

    for (int winner : winners)
    {
        response += this->getPlayerById(winner).username;
        response += " ";
    }

    return response;
}

void LinqServerThread::onMessage(ServerConnection *server, int client_id, std::string message)
{
    PlayerRequest request(message);
    if (request.type == ClientRequestType::CONNECT)
    {
        std::cout << "Client " << client_id << " connected with username " << request.args[0] << std::endl;
        game.addPlayer(client_id, request.args[0]);

        for (const Player &player : game.players)
        {
            std::string connection_message = game.generateConnectionMessageForPlayer(player.id);
            server->sendToClient(player.id, connection_message);
        }

        if (game.players.size() == 5)
        {
            game.assignRoles();
            game.shufflePlayers();
            for (const Player &player : game.players)
            {
                std::string start_message = game.generateStartMessage(player.id);
                server->sendToClient(player.id, "\n");
                server->sendToClient(player.id, start_message);
            }

            game.startGame();

            std::string play_message = game.generatePlayMessage(game.turn);
            for (const Player &player : game.players)
            {
                server->sendToClient(player.id, "\n");
                server->sendToClient(player.id, play_message);
            }
        }
    }
    if (request.type == ClientRequestType::WORD)
    {
        int player_id = std::stoi(request.args[0]);
        std::cout << game.getPlayerById(player_id).username << " said " << request.args[1] << std::endl;
        game.getPlayerById(player_id).words.push_back(request.args[1]);

        std::string said_message = game.generateSaidMessage(player_id);
        for (const Player &player : game.players)
        {
            server->sendToClient(player.id, "\n");
            server->sendToClient(player.id, said_message);
        }

        game.nextTurn();
        std::string play_message = game.generatePlayMessage(game.turn);
        for (const Player &player : game.players)
        {
            server->sendToClient(player.id, "\n");
            server->sendToClient(player.id, play_message);
        }
    }
    else if (request.type == ClientRequestType::CAST)
    {
        int player_id = std::stoi(request.args[0]);
        int target1_id = game.getPlayerByUsername(request.args[1]).id;
        int target2_id = game.getPlayerByUsername(request.args[2]).id;

        game.getPlayerById(player_id).votes.first = target1_id;
        game.getPlayerById(player_id).votes.second = target2_id;

        std::string cast_message = game.generateVotedMessage(player_id);
        for (const Player &player : game.players)
        {
            server->sendToClient(player.id, "\n");
            server->sendToClient(player.id, cast_message);
        }

        bool isFinished = game.nextTurn();
        if (isFinished)
        {
            std::string winners_message = game.generateWinMessage();
            for (const Player &player : game.players)
            {
                server->sendToClient(player.id, "\n");
                server->sendToClient(player.id, winners_message);
            }
            return;
        }
        else
        {
            std::string play_message = game.generatePlayMessage(game.turn);
            for (const Player &player : game.players)
            {
                server->sendToClient(player.id, "\n");
                server->sendToClient(player.id, play_message);
            }
        }
    }
    else
    {
        std::cout << "Client " << client_id << " sent " << message << std::endl;
    }
}

void LinqServerThread::onConnect(ServerConnection *server, int client_id)
{
    std::cout << "Client " << client_id << " connected" << std::endl;
}

void LinqServerThread::onDisconnect(ServerConnection *server, int client_id)
{
    std::cout << "Client " << client_id << " disconnected" << std::endl;
}

LinqServerThread::LinqServerThread(std::string ip, int port)
{
    this->server = new ServerConnection(ip, port);
}

LinqServerThread::~LinqServerThread()
{
    delete this->server;
}

void LinqServerThread::startThread(ServerStateHandler onStart)
{
    std::thread t([this, onStart]()
                  { this->server->start(
                        [this](ServerConnection *server, int client_id, std::string message)
                        {
                            this->onMessage(server, client_id, message);
                        },
                        [this](ServerConnection *server, int client_id)
                        {
                            this->onConnect(server, client_id);
                        },
                        [this](ServerConnection *server, int client_id)
                        {
                            this->onDisconnect(server, client_id);
                        },
                        onStart); });

    t.detach();
}
