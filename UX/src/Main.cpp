#include <iostream>
#include <random>
#include "./headers/LinqComponents.hpp"
#include <LinqServer.hpp>
#include <LinqClient.hpp>

#define NUMBER_OF_PLAYERS 5

#define MAIN_MENU 0
#define CONNECT_MENU 1
#define CREATE_GAME_MENU 2
#define CONNECTION_LOBBY 3
#define GAME_PAGE 4

LinqClient::ClientConnection *client = nullptr;
LinqClient::LinqClientGame *game = nullptr;

Flow::Window window;

// Setup Page
std::vector<Flow::Text *> players_display;
Flow::Text *port_text = nullptr;
std::map<std::string, std::string> players_words;

// Game Page
Flow::Image *game_background = nullptr;
Flow::Image *game_board = nullptr;

int startGameServer(int port, std::string username);
int startGameClient(int port, std::string username);
void onMessage(LinqClient::ClientConnection *client, std::string message);

int startGameServer(int port, std::string username)
{
	LinqServer::LinqServerThread *linq_server = new LinqServer::LinqServerThread("127.0.0.1", port);
	linq_server->startThread(
		[port, username](LinqServer::ServerConnection *)
		{
			std::cout << "Server Started" << std::endl;
			startGameClient(port, username);
		});

	return 0;
}

void onMessage(LinqClient::ClientConnection *client, std::string message)
{
	LinqClient::ServerRequest request(message);
	if (request.type == LinqClient::ServerRequestType::CONNECTED)
	{
		std::cout << "Connected to server" << std::endl;
		game->id = std::stoi(request.args[0]);
		game->players = std::vector<std::string>(request.args.begin() + 1, request.args.end());
		for (size_t i = 0; i < game->players.size(); i++)
		{
			players_display[i]->setText(game->players[i]);
		}
	}
	else if (request.type == LinqClient::ServerRequestType::START)
	{
		game->handleStartRequest(request);

		std::cout << std::string(100, '\n');
		std::cout << "Your role is " << (game->role == LinqClient::PlayerRole::COUNTER_SPY ? "Counter-Spy" : "Spy") << std::endl;

		if (game->role == LinqClient::PlayerRole::SPY)
			std::cout << "Your word is " << game->word << std::endl;

		std::cout << "Players in the game:" << std::endl;
		for (const std::string &player : game->players)
			if (player != game->username)
				std::cout << player << std::endl;
			else
				std::cout << player << " (You)" << std::endl;

		LinqComponents::Role component_role;
		if (game->role == LinqClient::PlayerRole::COUNTER_SPY)
			component_role = LinqComponents::Role::CounterSpy;
		else
			component_role = LinqComponents::Role::Spy;

		game_board = LinqComponents::createGameBoard(&window, game_background, game->username, component_role, game->word, GAME_PAGE);

		for (const std::string &player : game->players)
		{
			players_words[player] = "";
		}

		window.onNextRender([]()
							{
		window.removeComponentsByLabel("game-data-label");
		LinqComponents::createGameDataSection(
			&window, game_board, players_words, "", [](std::string output) {}, false, GAME_PAGE); });

		window.setCurrentPage(GAME_PAGE);
	}
	else if (request.type == LinqClient::ServerRequestType::PLAY)
	{
		if (std::stoi(request.args[0]) == game->id)
		{
			window.onNextRender([client]()
								{
			window.removeComponentsByLabel("game-data-label");
			LinqComponents::createGameDataSection(
				&window, game_board, players_words, "",
				[client](std::string output)
				{ client->sendMessage("WORD " + std::to_string(game->id) + " " + output); },
				true, GAME_PAGE); });
		}
		else
		{
			window.onNextRender([client, request]()
								{
				window.removeComponentsByLabel("game-data-label");
				LinqComponents::createGameDataSection(
					&window, game_board, players_words, request.args[1],
					[client](std::string output)
					{ client->sendMessage("WORD " + std::to_string(game->id) + " " + output); },
					false, GAME_PAGE); });
		}
	}
	else if (request.type == LinqClient::ServerRequestType::SAID)
	{
		players_words[request.args[0]] = players_words[request.args[0]] + " " + request.args[1];

		window.onNextRender([client, request]()
							{
			window.removeComponentsByLabel("game-data-label");
			LinqComponents::createGameDataSection(
				&window, game_board, players_words, "", [](std::string output) {}, false, GAME_PAGE); });
	}
	else
	{
		std::cout << "Unknown message: " << message << std::endl;
	}
}

int startGameClient(int port, std::string username)
{
	game = new LinqClient::LinqClientGame();
	game->username = username;
	client = new LinqClient::ClientConnection("127.0.0.1", port);
	client->connectToServer();
	client->onMessage(onMessage);
	std::string port_text_string = "Game open on port [" + std::to_string(port) + "]";
	if (port_text != nullptr)
		port_text->setText(port_text_string);

	client->sendMessage("CONNECT " + username);
	return 0;
}

int main(int argc, char *argv[])
{
	Flow::InitializeGUI();
	srand(time(NULL));

	// *****************
	// * Main Menu Page *
	// *****************
	window.setBackground({125, 125, 255, 255});
	window.setDimension({1360, 800});
	window.setResizable(true);
	window.setTitle("Linq : 1910");

	Flow::Image *main_menu_background = LinqComponents::setPageBackground(&window, MAIN_MENU, LinqComponents::MainBackgroundImagePath);
	Flow::Image *spy_board = LinqComponents::createMenuBoard(&window, main_menu_background, "Linq : 1910", MAIN_MENU);
	LinqComponents::createBoardNavigationButton(&window, spy_board, "Create Game", 1.5, MAIN_MENU, CREATE_GAME_MENU);
	LinqComponents::createBoardNavigationButton(&window, spy_board, "Connect", -1.5, MAIN_MENU, CONNECT_MENU);

	// *****************
	// * Create Game Page *
	// *****************

	Flow::Image *create_game_background = LinqComponents::setPageBackground(&window, CREATE_GAME_MENU, LinqComponents::MainBackgroundImagePath);
	Flow::Image *create_game_spy_board = LinqComponents::createMenuBoard(&window, create_game_background, "Create Game", CREATE_GAME_MENU);
	Flow::Text *port_input = LinqComponents::createInputFrame(&window, create_game_spy_board, "3000", -6, CREATE_GAME_MENU);
	Flow::Text *username_input = LinqComponents::createInputFrame(&window, create_game_spy_board, "Fred", -3, CREATE_GAME_MENU);
	LinqComponents::createBoardNavigationButton(&window, create_game_spy_board, "Back", 0, CREATE_GAME_MENU, MAIN_MENU);
	LinqComponents::createBoardActionButton(&window, create_game_spy_board, "Create", 3, CREATE_GAME_MENU, [port_input, username_input](Flow::Window *window, Flow::Component *component)
											{ std::cout << "Creating Game on port " << port_input->getText() << std::endl; 
											startGameServer(std::stoi(port_input->getText()), username_input->getText());
											  window->setCurrentPage(CONNECTION_LOBBY); });

	// *****************
	// * Connect Page *
	// *****************
	std::string random_player_name = "Player" + std::to_string(rand() % 1000);
	Flow::Image *connect_background = LinqComponents::setPageBackground(&window, CONNECT_MENU, LinqComponents::MainBackgroundImagePath);
	Flow::Image *connect_spy_board = LinqComponents::createMenuBoard(&window, connect_background, "Connect", CONNECT_MENU);
	Flow::Text *connect_port_input = LinqComponents::createInputFrame(&window, connect_spy_board, "3000", -6, CONNECT_MENU);
	Flow::Text *connect_username_input = LinqComponents::createInputFrame(&window, connect_spy_board, random_player_name, -3, CONNECT_MENU);

	LinqComponents::createBoardNavigationButton(&window, connect_spy_board, "Back", -1.5, CONNECT_MENU, MAIN_MENU);
	LinqComponents::createBoardActionButton(&window, connect_spy_board, "Connect", 1.5, CONNECT_MENU, [connect_port_input, connect_username_input](Flow::Window *window, Flow::Component *component)
											{
												std::cout << "Connecting to Game on port " << connect_port_input->getText() << std::endl;
												startGameClient(std::stoi(connect_port_input->getText()), connect_username_input->getText());
												window->setCurrentPage(CONNECTION_LOBBY); });

	// *****************
	// * Waiting for connection Page *
	// *****************

	Flow::Image *waiting_for_connection_background = LinqComponents::setPageBackground(&window, CONNECTION_LOBBY, LinqComponents::MainBackgroundImagePath);
	Flow::Image *waiting_for_connection_spy_board = LinqComponents::createMenuBoard(&window, waiting_for_connection_background, "Game Lobby", CONNECTION_LOBBY);
	port_text = LinqComponents::createTextLine(&window, waiting_for_connection_spy_board, "Game open on port [????]:", LinqComponents::OldStandardTTBold, 24, 3, CONNECTION_LOBBY);
	LinqComponents::createTextLine(&window, waiting_for_connection_spy_board, "Connected players:", LinqComponents::OldStandardTTBold, 24, 5, CONNECTION_LOBBY);
	for (size_t i = 0; i < NUMBER_OF_PLAYERS; i++)
	{
		players_display.emplace_back(LinqComponents::createTextLine(&window, waiting_for_connection_spy_board, "-", LinqComponents::OldStandardTT, 24, 7 + 2 * i, CONNECTION_LOBBY));
	}

	// *****************
	// * Game Page 	   *
	// *****************

	game_background = LinqComponents::setPageBackground(&window, GAME_PAGE, LinqComponents::GameBackgroundImagePath);

	window.setCurrentPage(MAIN_MENU);
	window.mainLoop();
	return 0;
}