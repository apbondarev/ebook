#include <algorithm>
#include <cassert>
#include <iostream>
#include <set>
#include <string>
#include <sstream>
#include <string_view>
#include <vector>

using namespace std;

class Domain {
public:
    explicit Domain(string domain): parts_() {
        int end = domain.size();
        int i = end - 1;
        while (i >= 0) {
            if (domain[i] == '.') {
                parts_.push_back(domain.substr(i + 1, end - i - 1));
                end = i;
            }
            --i;
        }
        parts_.push_back(domain.substr(i + 1, end - i - 1));
    }

    string ToString() const;

    const vector<string> GetParts() const {
        return parts_;
    }

    bool operator==(const Domain& rhs) const {
        return Equals(rhs);
    }

    bool operator!=(const Domain& rhs) const {
        return !Equals(rhs);
    }

    bool operator<(const Domain& rhs) const {
        return Less(rhs);
    }

    bool operator>(const Domain& rhs) const {
        return !Less(rhs);
    }

    bool IsSubdomain(const Domain& domain) const {
        size_t min_size = std::min(parts_.size(), domain.parts_.size());
        for (size_t i = 0; i < min_size; ++i) {
            if (parts_[i] != domain.parts_[i]) {
                return false;
            }
        }
        return parts_.size() >= domain.parts_.size();
    }

private:
    vector<string> parts_;

    bool Less(const Domain& rhs) const {
        return lexicographical_compare(parts_.begin(), parts_.end(), rhs.parts_.begin(), rhs.parts_.end());
    }

    bool Equals(const Domain& rhs) const {
        if (this == &rhs) {
            return true;
        }
        if (parts_.size() != rhs.parts_.size()) {
            return false;
        }
        for (size_t i = 0; i < parts_.size(); ++i) {
            if (parts_[i] != rhs.parts_[i]) {
                return false;
            }
        }
        return true;
    }
};

string Domain::ToString() const {
    string concat;
    for (auto iter = parts_.crbegin(); iter != parts_.crend(); ++iter) {
        if (!concat.empty()) {
            concat.push_back('.');
        }
        concat += *iter;
    }
    return concat;
}

template <typename Container>
void PrintContainer(ostream& output, Container&& container) {
    for (const auto& it : container) {
        output << it.ToString() << endl;
    }
}


class DomainChecker {
public:
    template <typename InputIt>
    DomainChecker(InputIt first, InputIt last): forbidden_domains_() {
        for (auto iter = first; iter != last; ++iter) {
            forbidden_domains_.push_back(*iter);
        }
        sort(forbidden_domains_.begin(), forbidden_domains_.end());

        auto new_last = unique(forbidden_domains_.begin(), forbidden_domains_.end(), [](const Domain& d1, const Domain& d2){
            return d2.IsSubdomain(d1);
        });

        forbidden_domains_.erase(new_last, forbidden_domains_.end());
    }

    bool IsForbidden(const Domain& domain) const {
        const auto iter = upper_bound(forbidden_domains_.begin(), forbidden_domains_.end(), domain);
        if (iter == forbidden_domains_.begin()) {
            return false;
        } else {
            auto pos = prev(iter);
            return domain.IsSubdomain(*pos);
        }
    }

private:
    vector<Domain> forbidden_domains_;
};

vector<Domain> ReadDomains(istream& input, size_t count) {
    std::vector<Domain> domains;
    for (size_t i = 0; i < count; ++i) {
        string line;
        getline(input, line);
        domains.push_back(Domain{move(line)});
    }
    return domains;
}

template <typename Number>
Number ReadNumberOnLine(istream& input) {
    string line;
    getline(input, line);

    Number num;
    std::istringstream(line) >> num;

    return num;
}

void TestCreateDomain() {
    vector<string> expectedOne = {"com"s};
    assert(Domain("com").GetParts() == expectedOne);

    vector<string> expectedMany = {"me"s, "maps"s, "m"s};
    assert(Domain("m.maps.me").GetParts() == expectedMany);
}

void TestCompareDomain() {
    assert(Domain("com") < Domain("gdz.ru"));
    assert(Domain("gdz.ru") == Domain("gdz.ru"));
    assert(!(Domain("gdz.ru") == Domain("gdz.com")));
    assert(Domain("m.maps.me") > Domain("maps.me"));
}

void TestIsSubDomain() {
    assert(Domain("gdz.com").IsSubdomain(Domain("com")));
    assert(!Domain("com").IsSubdomain(Domain("gdz.com")));
    assert(Domain("m.maps.me").IsSubdomain(Domain("maps.me")));
    assert(!Domain("gdz.ru").IsSubdomain(Domain("maps.me")));
}

void TestDomainChecker() {
    vector<Domain> forbidden_domains = {
        Domain("gdz.ru"),
        Domain("maps.me"),
        Domain("m.gdz.ru"),
        Domain("com")
    };
    DomainChecker checker(forbidden_domains.begin(), forbidden_domains.end());
    assert(checker.IsForbidden(Domain("gdz.ru")));
    assert(checker.IsForbidden(Domain("gdz.com")));
    assert(checker.IsForbidden(Domain("m.maps.me")));
    assert(checker.IsForbidden(Domain("alg.m.gdz.ru")));
    assert(checker.IsForbidden(Domain("maps.com")));
    assert(!checker.IsForbidden(Domain("maps.ru")));
    assert(!checker.IsForbidden(Domain("gdz.ua")));
}

void TestDomainChecker2() {
    vector<Domain> forbidden_domains = {Domain("c"s), Domain("b.c"s)};
    DomainChecker checker(forbidden_domains.begin(), forbidden_domains.end());
    assert(!checker.IsForbidden(Domain("a")));
    assert(checker.IsForbidden(Domain("c")));
    assert(checker.IsForbidden(Domain("d.c")));
    assert(checker.IsForbidden(Domain("b.c")));
    assert(checker.IsForbidden(Domain("a.b.c")));
    assert(!checker.IsForbidden(Domain("d")));
}


int main() {
    // TestCreateDomain();
    // TestCompareDomain();
    // TestIsSubDomain();
    // TestDomainChecker();
    // TestDomainChecker2();

    const std::vector<Domain> forbidden_domains = ReadDomains(cin, ReadNumberOnLine<size_t>(cin));
    DomainChecker checker(forbidden_domains.begin(), forbidden_domains.end());

    const std::vector<Domain> test_domains = ReadDomains(cin, ReadNumberOnLine<size_t>(cin));
    for (const Domain& domain : test_domains) {
        cout << (checker.IsForbidden(domain) ? "Bad"sv : "Good"sv) << endl;
    }
}
