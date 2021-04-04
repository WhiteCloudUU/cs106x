/**
 * File: word-ladder.cpp
 * ---------------------
 * Implements a program to find word ladders connecting pairs of words.
 */

#include <iostream>
using namespace std;

#include "console.h"
#include "lexicon.h"
#include "strlib.h"
#include "simpio.h"

#include "vector.h"
#include "queue.h"
#include "set.h"

static string getWord(const Lexicon& english, const string& prompt) {
    while (true) {
        string response = trim(toLowerCase(getLine(prompt)));
        if (response.empty() || english.contains(response)) return response;
        cout << "Your response needs to be an English word, so please try again." << endl;
    }
}



static void generateLadder(const Lexicon& english, const string& start, const string& end) {
    cout << "Here's where you'll search for a word ladder connecting \"" << start << "\" to \"" << end << "\"." << endl;

    Vector<string> word_ladder;
    word_ladder.add(start);
    Set<string> words_in_use;
    words_in_use.add(start);
    Queue<Vector<string>> queue;
    queue.add(word_ladder);

    string top_word;
    while (!queue.isEmpty()) {
        word_ladder = queue.dequeue();
        top_word = word_ladder[word_ladder.size() - 1];
        if (top_word == end) {
            break;
        }

        for (int i = 0; i < top_word.length(); i++) {
            for (int j = 0; j < 26; j++) {
                if (j != top_word[i] - 'a') {
                    string top_word_copy = top_word;
                    top_word_copy[i] = 'a' + j;
                    if (english.contains(top_word_copy) && !words_in_use.contains(top_word_copy)) {
                        words_in_use.add(top_word_copy);
                        Vector<string> word_ladder_copy = word_ladder;
                        word_ladder_copy.add(top_word_copy);
                        queue.add(word_ladder_copy);
                    }
                }
            }
        }

    }

    cout << "Found ladder: ";
    for (int i = 0; i < word_ladder.size() - 1; i++) {
        cout << word_ladder[i] << " --> ";
    }
    cout << word_ladder[word_ladder.size() - 1] << endl;

}

/*
 * Function: find_one_hop_words
 * ----------------------------
 * for each word that differs by one char from top word
 * and exists in the lexicon
 * and has not already been used in some other ladder,
 * 1. create copy of partial ladder,
 * 2. extend this ladder by pushing new word on top,
 * 3. enqueue this ladder at end of queue
 */
void find_one_hop_words(string word, Vector<string> word_ladder) {
    return;
}



static const string kEnglishLanguageDatafile = "dictionary.txt";
static void playWordLadder() {
    Lexicon english(kEnglishLanguageDatafile);
    while (true) {
        string start = getWord(english, "Please enter the source word [return to quit]: ");
        if (start.empty()) break;
        string end = getWord(english, "Please enter the destination word [return to quit]: ");
        if (end.empty()) break;
        generateLadder(english, start, end);
    }
}

int main() {
    cout << "Welcome to the CS106 word ladder application!" << endl << endl;
    playWordLadder();
    cout << "Thanks for playing!" << endl;
    return 0;
}
