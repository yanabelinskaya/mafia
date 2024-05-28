#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <map>

enum Role { Mafia, Civilian, Doctor, Detective };

class Player {
public:
    Player(std::string name, Role role) : name(std::move(name)), role(role), alive(true) {}

    std::string getName() const { return name; }
    Role getRole() const { return role; }
    bool isAlive() const { return alive; }
    void kill() { alive = false; }

private:
    std::string name;
    Role role;
    bool alive;
};

class Game {
public:
    Game(std::vector<std::string> playerNames) {
        srand(static_cast<unsigned>(time(0)));
        assignRoles(std::move(playerNames));
    }

    void play() {
        showPlayers();
        while (true) {
            if (isGameOver()) break;
            nightPhase();
            if (isGameOver()) break;
            dayPhase();
        }
        announceWinner();
    }

private:
    std::vector<Player> players;
    int mafiaCount;
    int civilianCount;

    void assignRoles(std::vector<std::string> playerNames) {
        int totalPlayers = playerNames.size();
        mafiaCount = totalPlayers / 3;  // Примерно 1/3 от общего числа игроков
        if (mafiaCount == 0) mafiaCount = 1;  // Минимум 1 мафия

        int doctorCount = 1;
        int detectiveCount = 1;
        int civilianCount = totalPlayers - (mafiaCount + doctorCount + detectiveCount);

        std::vector<Role> roles;
        roles.insert(roles.end(), mafiaCount, Mafia);
        roles.insert(roles.end(), doctorCount, Doctor);
        roles.insert(roles.end(), detectiveCount, Detective);
        roles.insert(roles.end(), civilianCount, Civilian);

        std::random_shuffle(roles.begin(), roles.end());

        for (size_t i = 0; i < playerNames.size(); ++i) {
            players.emplace_back(playerNames[i], roles[i]);
        }

        this->mafiaCount = std::count_if(players.begin(), players.end(), [](Player& p) { return p.getRole() == Mafia; });
        this->civilianCount = players.size() - this->mafiaCount;

        std::cout << "Количество мафий: " << mafiaCount << std::endl;
    }

    void showPlayers() {
        std::cout << "Список игроков:\n";
        for (size_t i = 0; i < players.size(); ++i) {
            std::cout << i << ": " << players[i].getName() << std::endl;
        }
    }

    void nightPhase() {
        std::cout << "\nФаза ночи:\n";
        int mafiaIndex = getMafiaIndex();
        int mafiaTarget = getPlayerChoice("Мафия выбирает, кого убить: ", false);
        int doctorSave = getPlayerChoice("Доктор выбирает, кого лечить: ", false);
        detectiveAction();

        if (mafiaIndex != -1 && mafiaTarget != doctorSave) {
            players[mafiaTarget].kill();
            --civilianCount;
            std::cout << players[mafiaTarget].getName() << " был убит мафией.\n";
        }
        else {
            std::cout << "Доктор спас игрока.\n";
        }
    }

    int getMafiaIndex() {
        for (size_t i = 0; i < players.size(); ++i) {
            if (players[i].getRole() == Mafia && players[i].isAlive()) {
                return i;
            }
        }
        return -1; // Если мафия не найдена
    }

    void dayPhase() {
        std::cout << "\nФаза дня:\n";
        int lynchTarget = dayVote();
        players[lynchTarget].kill();

        if (players[lynchTarget].getRole() == Mafia) {
            --mafiaCount;
            std::cout << players[lynchTarget].getName() << " был повешен. Он был мафией!\n";
            if (mafiaCount > 0) {
                std::cout << "Осталось мафий: " << mafiaCount << ". Начинается следующая ночь.\n";
                return; // Начинаем следующую ночь
            }
        }
        else {
            --civilianCount;
            std::cout << players[lynchTarget].getName() << " был повешен. Он был мирным жителем.\n";
        }
    }

    int getPlayerChoice(const std::string& prompt, bool hideRole) {
        int choice;
        while (true) {
            std::cout << prompt;
            std::cin >> choice;
            if (choice >= 0 && choice < players.size() && players[choice].isAlive()) {
                if (hideRole && players[choice].getRole() != Mafia) {
                    std::cout << "Ошибка! Только мафия может делать этот выбор.\n";
                }
                else {
                    return choice;
                }
            }
            else {
                std::cout << "Некорректный выбор. Пожалуйста, выберите снова.\n";
            }
        }
    }

    void detectiveAction() {
        int investigate = getPlayerChoice("Детектив выбирает, кого проверить: ", false);
        std::cout << players[investigate].getName() << " - "
            << (players[investigate].getRole() == Mafia ? "мафия." : "не мафия.")
            << "\n";
    }

    int dayVote() {
        std::vector<int> votes(players.size(), 0);

        std::cout << "Голосование начинается. Игроки голосуют за подозреваемого:\n";
        for (size_t i = 0; i < players.size(); ++i) {
            if (players[i].isAlive()) {
                int vote = getPlayerChoice(players[i].getName() + " голосует за: ", false);
                ++votes[vote];
            }
        }

        return std::distance(votes.begin(), std::max_element(votes.begin(), votes.end()));
    }

    bool isGameOver() {
        // Игра продолжается, если хотя бы один мафиози еще жив
        return mafiaCount == 0 || (mafiaCount == civilianCount);
    }

    void announceWinner() {
        if (mafiaCount == 0) {
            std::cout << "Мирные жители победили!\n";
        }
        else {
            std::cout << "Мафия победила!\n";
        }
    }
};

int main() {
    setlocale(LC_ALL, "Rus");
    std::vector<std::string> playerNames = { "Айжа", "Янка", "Сашка", "Ксю", "Женька", "Вика" };
    Game game(playerNames);
    game.play();
    return 0;
}