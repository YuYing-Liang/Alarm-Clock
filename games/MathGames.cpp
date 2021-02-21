/*
    MathGames.h - this library provides code for the counting and mental math games of the alarm clock
*/

#include "MathGames.h"
#include <cmath>
#include <string>

MathGames::MathGames(int type) {
    if(type == MENTAL_MATH) {
        operation = rand() % 4;
        numbers[0] = rand() % 100;
        numbers[1] = rand() % 100;
        switch (operation) {
            case ADD: answer = numbers[0] + numbers[1]; break;
            case SUB: answer = numbers[0] - numbers[1]; break;
            case MULT: answer = numbers[0] * numbers[1]; break;
            // case DIV: answer = numbers[0]/numbers[1]; break;
            default: answer = 0; break;
        }
    }else if(type == COUNTING) {
        resetCounting();
        shuffle();
    }
}

void MathGames::shuffle() {
    for(int i=0; i < 1000; i++) {
        int idx1 = rand() % 10;
        int idx2 = rand() % 10;

        int temp = num_order[idx1];
        num_order[idx1] = num_order[idx2];
        num_order[idx2] = temp;
    }
}

void MathGames::processAnswer(int ans) {
    // TODO: finish
    if(ans == answer){

    }
}

char* MathGames::getQuestion() {
    char q[16];
    char op = op_chars[operation];
    sprintf(q, "What is %d %c %d?", numbers[0], op, numbers[1]);
    return q;
}

void MathGames::resetCounting() {
    for(int i=0; i < LEN_NUMS; i++){
        num_order[i] = i;
    }
}

