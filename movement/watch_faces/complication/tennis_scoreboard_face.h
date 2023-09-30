/*
 * MIT License
 *
 * Copyright (c) 2023 Jaime Laborda
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef TENNIS_SCOREBOARD_FACE_H_
#define TENNIS_SCOREBOARD_FACE_H_

#include "movement.h"

/*
 * A DESCRIPTION OF YOUR WATCH FACE
 *
 * and a description of how use it
 *
 */

/*
* TODO:
*  - Add tiebreak logic
*  - Add serve logic
*  - Solve issue match reseting on each face activation
*/

//uint8_t lut_points = {"0", "15", "30", "40", "Ad"};
//typedef enum WatchIndicatorSegment { -> Usar este tipo de struct para indicar los puntos
//    WATCH_INDICATOR_SIGNAL = 0, ///< The hourly signal indicator; also useful for indicating that sensors are on.
//    WATCH_INDICATOR_BELL,       ///< The small bell indicating that an alarm is set.
//    WATCH_INDICATOR_PM,         ///< The PM indicator, indicating that a time is in the afternoon.
//    WATCH_INDICATOR_24H,        ///< The 24H indicator, indicating that the watch is in a 24-hour mode.
//    WATCH_INDICATOR_LAP         ///< The LAP indicator; the F-91W uses this in its stopwatch UI.
//} WatchIndicatorSegment;

typedef struct {
    uint8_t points;
    uint8_t games;
    //bool serve;     //If the player serves or not. TODO: It only requires 1 boolean because only requirement is if P1 serves or not
} tennis_player_t;  //Player 1 and player 2 cannot serve at the same time

typedef struct {
    tennis_player_t player[2];
    bool player_winner; //What player won the set (0 -> player_1, 1 -> player_2)
    bool tiebreak_active;    //0 -> no tiebreak, 1 -> tiebreak active
} tennis_set_t;

typedef struct {
    // Anything you need to keep track of, put it here!
    //uint8_t test_counter;
    uint8_t current_set;  //from 1 to 3
    tennis_set_t set[4]; //4 sets, 0 is not used
    uint8_t games_per_set;
    uint8_t number_of_sets; //Best of 3
    bool player1_serves;
    uint8_t player_sets_won[2]; // Counter to keep track of how many sets each player has won
    uint8_t match_winner; //What player won the match (0 -> player_1, 1 -> player_2)
    bool results_screen;  //Variable used to store if results screen is selected
    
} tennis_scoreboard_state_t;

void tennis_scoreboard_face_setup(movement_settings_t *settings, uint8_t watch_face_index, void ** context_ptr);
void tennis_scoreboard_face_activate(movement_settings_t *settings, void *context);
bool tennis_scoreboard_face_loop(movement_event_t event, movement_settings_t *settings, void *context);
void tennis_scoreboard_face_resign(movement_settings_t *settings, void *context);
void render_result_screen (tennis_scoreboard_state_t *state);
void render_match_screen (tennis_scoreboard_state_t *state) ;
void print_counter_test(tennis_scoreboard_state_t *state);
void player_wins_set(tennis_scoreboard_state_t *state, uint8_t winning_player);
void player_wins_game(tennis_scoreboard_state_t *state, uint8_t winning_player);
void player_wins_point(tennis_scoreboard_state_t *state, uint8_t winning_player);
void reset_match(tennis_scoreboard_state_t *state);

#define tennis_scoreboard_face ((const watch_face_t){ \
    tennis_scoreboard_face_setup, \
    tennis_scoreboard_face_activate, \
    tennis_scoreboard_face_loop, \
    tennis_scoreboard_face_resign, \
    NULL, \
})

#endif // TENNIS_SCOREBOARD_FACE_H_

