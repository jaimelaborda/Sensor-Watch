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
 * This watch face is a tennis scoreboard. It keeps track of the score of a tennis match.
 * It has 3 screens:
 * - Match screen: Shows the current score of the match
 * - Results screen: Shows the results of the match
 * - Settings screen: Allows to change the number of sets and games per set (TODO, not implemented yet)
 * 
 * When the watch face is activated, it shows the match screen. User can change between screens by pressing the LIGHT button.
 * Press MODE button when Player 1 wins a point. Press the ALARM button when Player 2 wins a point.
 * A game is won when a player reaches 4 points and has 2 points more than the other player.
 * A set is won when a player wins 6 games and has 2 games more than the other player.
 * A match is won when a player wins 2 sets (TODO: this is by default, but can be changed in the settings screen)
 * When the match is over, the results screen is shown. (TODO, not implemented yet)
 * Result screen can also accessed by pressing LIGHT button. In this screen, pressing ALARM button changes the set (1, 2 or 3)
 * In the results screen, long pressing MODE button resets the match.
 * 
 * IMPORTANT: To go to the next watch face, long press LIGHT button. This is done on purpose to avoid changing watch face by mistake.
 */

/*
* TODO:
*  - Add tiebreak logic
*  - Add serve logic
*  - Solve issue match reseting on each face activation
*  - Solve bug when substracting points if player has 0 points
*  - Add settings screen
*  - Show the results screen when the match is over and blink the winner
*/

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

