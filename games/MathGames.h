/*
    MathGames.h - this library provides code for the counting and mental math games of the alarm clock
*/

#ifndef MathGames_h
#define MathGames_h

enum operations {
    ADD = 0,
    SUB = 1,
    MULT = 2,
    // DIV = 3
};

enum gameType {
    MENTAL_MATH = 0,
    COUNTING = 1
};

#define LEN_NUMS 10

// #include "Arduino.h"
#include <string>

class MathGames {
    private:
        // mental math games
        int operation;
        int numbers[2];
        int answer;
        char op_chars[3] = {'+','-','×'};

        // counting game
        int num_order[LEN_NUMS];

        void resetCounting();
        void resetMentalMath();
        void shuffle();

    public:
        // set's mental math or counting question based on type
        MathGames(int type);
        
        // Mental Math Game
        void processAnswer(int ans);
        char* getQuestion();

        // Counting Game 
        void processNumOrder(int ans[]);
};
#endif
