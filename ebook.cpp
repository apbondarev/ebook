#include <cassert>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <stdexcept>
#include <vector>

/* В общем и целом - всё в порядке, всё понятно, как устроено и как работает. 
   Я не понял, зачем ты используешь шаблонный тип в классе, 
   и мне показались излишними проверки-ассерты - как-то сразу не придумывается, как
   нужно извратиться с входными данными, чтобы они сработали - 
   тут мне за авторами ЯП не угнаться :-) Ну, пожалуй, я бы еще "внутренности" реакции 
   на запрос CHEER спрятал внутрь класса - тогда класс внутри сможет изменяться, а 
   наружный интерфейс не затронется.
   Т.е., в итоге - только мелкие придирки, ничего серьезного :-) */

template <typename Participant> // А зачем тут шаблонный тип? 
                                // И без него всё вроде работает. 
                                // В крайнем случае псевдоним для int через using - не?
class RacePositions {           // Читательские гонки - приколист :-)
public:
    void UpdatePosition(Participant id, int new_position) {
        assert(new_position >= 0); // Нулевая страница/позиция особо смысла не имеет - может, лучше начинать с 1?
        if (static_cast<int>(positions_.size()) - 1 < new_position) { // а не проще сразу зарезервировать 1000 страниц, 
            positions_.resize(new_position + 1);                       // как в задании, и забыть про рисайз?
        }
        auto participant_iter = participant_to_position_.find(id);
        if (participant_iter == participant_to_position_.end()) {
            participant_to_position_[id] = new_position;
        } else {
            int old_position = std::exchange(participant_iter->second, new_position);
            --positions_[old_position];
            assert(positions_[old_position] >= 0); // если доверять входным данным или проверять их сразу, то эти ассерты не понадобятся.
        }
        ++positions_[new_position];
    }

    int GetPlace(Participant id) const { // UpdatePosition <-> GetPosition? Сервисная функция, снаружи не используется - стоит ее в private убрать?
        if (participant_to_position_.count(id) == 0) {
            return -1;
        }
        int position = participant_to_position_.at(id);
        assert(position >= 0); // ну если мы раньше всё делали правильно, то с чего сейчас ему в минус уходить?
        assert(static_cast<int>(positions_.size()) - 1 >= position);// не понял, что тут проверяется, целостность базы данных?

        int sum = 0;
        for (size_t i = 0; i < static_cast<size_t>(position); ++i) {
            sum += positions_[i];
        }
        return sum;
    }

    double GetFractionLessThan(Participant id) {
        int place = GetPlace(id);
        if (place < 0) {
            return 0;
        }
        if (participant_to_position_.size() == 1) {
            return 1;
        }
        return static_cast<double>(place) / (participant_to_position_.size() - 1);
    }

private:
    std::map<Participant, int> participant_to_position_;
    std::vector<int> positions_;
};

int main() {
    using namespace std::string_literals;

    RacePositions<int> book_reader_positions;
    int queries_count;

    std::cin >> queries_count;

    std::string command;
    int reader_id;
    int page;
    for (int i = 0; i < queries_count; ++i) {
        std::cin >> command;
        if (command == "READ"s) {
            std::cin >> reader_id;
            std::cin >> page;
            book_reader_positions.UpdatePosition(reader_id, page);
        } else if (command == "CHEER"s) {
            std::cin >> reader_id;
            std::cout << std::setprecision(6) << book_reader_positions.GetFractionLessThan(reader_id) << std::endl;
        } else {
            throw std::invalid_argument("unknown command: "s + command);
        }
    }
}
