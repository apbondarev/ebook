#include <cassert>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <stdexcept>
#include <vector>

template <typename Participant>
class RacePositions {
public:
    void UpdatePosition(Participant id, int new_position) {
        assert(new_position >= 0);
        if (static_cast<int>(positions_.size()) - 1 < new_position) {
            positions_.resize(new_position + 1);
        }
        auto participant_iter = participant_to_position_.find(id);
        if (participant_iter == participant_to_position_.end()) {
            participant_to_position_[id] = new_position;
        } else {
            int old_position = std::exchange(participant_iter->second, new_position);
            --positions_[old_position];
            assert(positions_[old_position] >= 0);
        }
        ++positions_[new_position];
    }

    int GetPlace(Participant id) const {
        if (participant_to_position_.count(id) == 0) {
            return -1;
        }
        int position = participant_to_position_.at(id);
        assert(position >= 0);
        assert(static_cast<int>(positions_.size()) - 1 >= position);

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
