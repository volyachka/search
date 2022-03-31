#include "search.h"
#include <string>
#include <set>
#include <cmath>
#include <map>
bool Comparator(std::pair<double, size_t> &a, std::pair<double, size_t> &b) {
    if (a.first == b.first) {
        return a.second < b.second;
    }
    return a.first > b.first;
}

std::vector<std::string_view> Search(std::string_view text, std::string_view query, size_t results_count) {
    std::vector<std::string_view> answer;
    std::set<std::string> words_in_query;
    std::map<std::string, size_t> number_of_matching;
    std::map<size_t, std::multiset<std::string>> suitable_words_in_request;
    std::vector<std::pair<size_t, size_t>> border_of_string;
    std::vector<std::pair<double, size_t>> tf_idf;
    std::vector<size_t> number_of_words_in_string;
    size_t number_of_string = 0;
    size_t begin_of_string = 0;
    size_t number_of_words = 0;
    std::string word;
    for (auto &letter : query) {
        if (std::isalpha(letter)) {
            word += tolower(letter);
        } else {
            words_in_query.insert(word);
            word = "";
        }
    }
    words_in_query.insert(word);
    word = "";
    for (auto i = 0; i < text.size(); ++i) {
        if (std::isalpha(text[i])) {
            word += tolower(text[i]);
        } else {
            if (words_in_query.contains(word)) {
                suitable_words_in_request[number_of_string].insert(word);
            }
            if (!word.empty()) {
                number_of_words += 1;
            }
            word = "";
        }
        if (text[i] == '\n') {
            for (auto &elem : words_in_query) {
                if (suitable_words_in_request[number_of_string].contains(elem)) {
                    number_of_matching[elem] += 1;
                }
            }
            border_of_string.emplace_back(std::make_pair(begin_of_string, i - begin_of_string));
            ++number_of_string;
            begin_of_string = i + 1;
            number_of_words_in_string.push_back(number_of_words);
            number_of_words = 0;
        }
    }
    border_of_string.emplace_back(std::make_pair(begin_of_string, text.size() - 1));
    ++number_of_string;
    number_of_words_in_string.push_back(number_of_words);
    number_of_words = 0;
    for (auto i = 0; i < number_of_string; ++i) {
        double tfidf = 0;
        for (auto &suitable_word : suitable_words_in_request[i]) {
            tfidf += log(double(number_of_string) / double(number_of_matching[suitable_word])) /
                     double(number_of_words_in_string[i]);
        }
        tf_idf.emplace_back(std::make_pair(tfidf, i));
    }
    std::sort(tf_idf.begin(), tf_idf.end(), Comparator);
    for (size_t i = 0; i < results_count; ++i) {
        if (tf_idf[i].first <= 0) {
            break;
        }
        answer.push_back(
            text.substr(border_of_string[tf_idf[i].second].first, border_of_string[tf_idf[i].second].second));
    }
    return answer;
}