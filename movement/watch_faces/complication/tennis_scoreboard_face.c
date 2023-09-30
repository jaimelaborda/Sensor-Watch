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

#include <stdlib.h>
#include <string.h>
#include "tennis_scoreboard_face.h"

#define DEBUG_STATUS

#define PLAYER_1 0
#define PLAYER_2 1

#ifdef DEBUG_STATUS
#define DEBUG(format, ...) \
    printf("DEBUG: " format "\n", ##__VA_ARGS__)
#else
#define DEBUG(format, ...) {}
#endif

void tennis_scoreboard_face_setup(movement_settings_t *settings, uint8_t watch_face_index, void ** context_ptr) {
    (void) settings;
    if (*context_ptr == NULL) {
        *context_ptr = malloc(sizeof(tennis_scoreboard_state_t));
        memset(*context_ptr, 0, sizeof(tennis_scoreboard_state_t));
        // Do any one-time tasks in here; the inside of this conditional happens only at boot.
        DEBUG("Setup tennis face");
    }
    // Do any pin or peripheral setup here; this will be called whenever the watch wakes from deep sleep.
}

void tennis_scoreboard_face_activate(movement_settings_t *settings, void *context) {
    (void) settings;
    tennis_scoreboard_state_t *state = (tennis_scoreboard_state_t *)context;

    reset_match(state);

    DEBUG("Enter face activate...");
}

// print counter index at the center of display.
void print_counter_test(tennis_scoreboard_state_t *state) {
    char buf[14];
    //sprintf(buf, "TE    %02d", state->test_counter); // center of LCD display
    watch_display_string(buf, 0);
}

void player_wins_set(tennis_scoreboard_state_t *state, uint8_t winning_player) {

    DEBUG("Player %d wins the set number %d", winning_player + 1, state->current_set);

    // Add set to the winning player
    state->set[state->current_set].player_winner = winning_player;
    state->player_sets_won[winning_player]++;

    if (state->player_sets_won[winning_player] == (state->number_of_sets + 1) / 2) {
      state->match_winner = winning_player;
      DEBUG("Player %d wins the match", winning_player + 1);
      return;
    }

    // Set current_set to the next set
    state->current_set++;
}

void player_wins_game(tennis_scoreboard_state_t *state, uint8_t winning_player) {
    uint8_t losing_player = (winning_player + 1) % 2;

    DEBUG("Player %d wins a game", winning_player + 1);

    // Set points to 0 to initialize next game
    state->set[state->current_set].player[winning_player].points = 0;
    state->set[state->current_set].player[losing_player].points = 0;

    // Add game to the winning player
    state->set[state->current_set].player[winning_player].games++;

    if (
        state->set[state->current_set].player[winning_player].games >=
        state->games_per_set
    ) {
      // oponent will have at least 2 games difference
      if (
        state->set[state->current_set].player[winning_player].games -
          state->set[state->current_set].player[losing_player].games >=
          2
      )
        player_wins_set(state, winning_player);
      else if (
        state->set[state->current_set].player[winning_player].games ==
        state->games_per_set && 
        state->set[state->current_set].player[losing_player].games ==
        state->games_per_set
      )
        state->set[state->current_set].tiebreak_active = true;
    }
}

void player_wins_point(tennis_scoreboard_state_t *state, uint8_t winning_player) {
    uint8_t losing_player = (winning_player + 1) % 2;

    DEBUG("Player %d wins a point", winning_player + 1);

    // 0 to 30
    if (state->set[state->current_set].player[winning_player].points <= 2)
        state->set[state->current_set].player[winning_player].points++;
    // 40
    else if (state->set[state->current_set].player[winning_player].points == 3) {
    // l = 40 - both had 40
    if (state->set[state->current_set].player[losing_player].points == 3)
        state->set[state->current_set].player[winning_player].points++;
    // l = AD
    else if (state->set[state->current_set].player[losing_player].points == 4)
        state->set[state->current_set].player[losing_player].points--;
    else player_wins_game(state, winning_player);
    }
    // AD
    else if (state->set[state->current_set].player[winning_player].points == 4)
    player_wins_game(state, winning_player);
}

void reset_match(tennis_scoreboard_state_t *state){
    DEBUG("Resseting match");

    memset(state, 0, sizeof(tennis_scoreboard_state_t));

    state->results_screen = 0;

    state->current_set = 1;
    state->games_per_set = 6;
    state->number_of_sets = 3; // Best of 3
    state->player_sets_won[PLAYER_1] = 0;
    state->player_sets_won[PLAYER_2] = 0;
    state->match_winner = 0;
    state->set[state->current_set].player[PLAYER_1].points = 0;
    state->set[state->current_set].player[PLAYER_2].points = 0;
    state->set[state->current_set].player[PLAYER_1].games = 0;
    state->set[state->current_set].player[PLAYER_2].games = 0;
    
    state->player1_serves = true;
}

void render_match_screen(tennis_scoreboard_state_t *state) {
    char buf[14];
    char points_string[5][3] = {"00\0", "15\0", "30\0", "40\0", "Ad\0"};

    // Set indicators
    watch_clear_indicator(WATCH_INDICATOR_LAP);
    watch_set_colon();
    
    // Format match screen
    //            0123 45678 9
    sprintf(buf, "TE %d%s%s%d%d", 
                                state->current_set,  
                                &points_string[state->set[state->current_set].player[PLAYER_1].points], 
                                &points_string[state->set[state->current_set].player[PLAYER_2].points], 
                                state->set[state->current_set].player[PLAYER_1].games, 
                                state->set[state->current_set].player[PLAYER_2].games
                                ); 
    DEBUG("Match Screen Rendering %s", buf);
    
    watch_display_string(buf, 0);
    if(state->player1_serves) watch_set_indicator(WATCH_INDICATOR_BELL);
                         else watch_clear_indicator(WATCH_INDICATOR_BELL);
}

void render_result_screen (tennis_scoreboard_state_t *state) {
    char buf[14];

    // Set indicators
    watch_clear_colon();
    watch_set_indicator(WATCH_INDICATOR_LAP);

    // Format results screen
    //            0123  45  67  89
    sprintf(buf, "TE %1d %1d-%1d  ", 
                                state->current_set, 
                                state->set[state->current_set].player[PLAYER_1].games, 
                                state->set[state->current_set].player[PLAYER_2].games 
                                ); 
    DEBUG("Result Screen Rendering %s", buf);

    watch_display_string(buf, 0);
}

bool tennis_scoreboard_face_loop(movement_event_t event, movement_settings_t *settings, void *context) {
    tennis_scoreboard_state_t *state = (tennis_scoreboard_state_t *)context;

    switch (event.event_type) {
        case EVENT_ACTIVATE:
            if(state->results_screen) render_result_screen(state);
                                else  render_match_screen(state);
            break;
        case EVENT_TICK:
            // If needed, update your display here.
            break;
        case EVENT_MODE_BUTTON_UP: 
            if(state->results_screen)
            {
                // Render Results screen
                render_result_screen(state);
            } else 
            {
                // Match logic
                player_wins_point(state, PLAYER_1);
                
                // Render Match screen
                render_match_screen(state);
            }
            break;
        case EVENT_MODE_LONG_PRESS: //the behaviour changes depending on results_screen variable
            if(state->results_screen) 
            {
                // Reset match
                reset_match(state);
                render_result_screen(state);
            }else  
            {
                render_match_screen(state);
                state->set[state->current_set].player[PLAYER_1].points--; //TODO: Needs to be improved
            }
            break;
        case EVENT_ALARM_BUTTON_UP:
            if(state->results_screen)
            {
                // Results screen logic
                if(state->current_set == 3)
                {
                    state->current_set = 1;
                } else
                {
                    state->current_set++;
                }
                DEBUG("Current set: %d", state->current_set);

                // Render Results screen
                render_result_screen(state);
            } else 
            {
                // Match logic
                player_wins_point(state, PLAYER_2);
                
                // Render Match screen
                render_match_screen(state);
            }
            break;
        case EVENT_ALARM_LONG_PRESS:
            if(state->results_screen) 
            {
                render_result_screen(state);
            }else  
            {
                render_match_screen(state);
                state->set[state->current_set].player[PLAYER_2].points--; //TODO: Needs to be improved
            }
            break;
        case EVENT_LIGHT_BUTTON_UP:
            state->results_screen = !state->results_screen;
            if(state->results_screen) render_result_screen(state);
                                else  render_match_screen(state);
            break;
        case EVENT_LIGHT_BUTTON_DOWN:
            // Empty EVENT_LIGHT_BUTTON_DOWN case so that LED is not iluminated
            break;
        case EVENT_LIGHT_LONG_UP:
            movement_move_to_next_face();
            break;
        case EVENT_TIMEOUT:
            // Your watch face will receive this event after a period of inactivity. If it makes sense to resign,
            // you may uncomment this line to move back to the first watch face in the list:
            // movement_move_to_face(0);
            break;
        case EVENT_LOW_ENERGY_UPDATE:
            // If you did not resign in EVENT_TIMEOUT, you can use this event to update the display once a minute.
            // Avoid displaying fast-updating values like seconds, since the display won't update again for 60 seconds.
            // You should also consider starting the tick animation, to show the wearer that this is sleep mode:
            // watch_start_tick_animation(500);
            break;
        default:
            // Movement's default loop handler will step in for any cases you don't handle above:
            // * EVENT_LIGHT_BUTTON_DOWN lights the LED
            // * EVENT_MODE_BUTTON_UP moves to the next watch face in the list
            // * EVENT_MODE_LONG_PRESS returns to the first watch face (or skips to the secondary watch face, if configured)
            // You can override any of these behaviors by adding a case for these events to this switch statement.
            return movement_default_loop_handler(event, settings);
    }

    // return true if the watch can enter standby mode. Generally speaking, you should always return true.
    // Exceptions:
    //  * If you are displaying a color using the low-level watch_set_led_color function, you should return false.
    //  * If you are sounding the buzzer using the low-level watch_set_buzzer_on function, you should return false.
    // Note that if you are driving the LED or buzzer using Movement functions like movement_illuminate_led or
    // movement_play_alarm, you can still return true. This guidance only applies to the low-level watch_ functions.
    return true;
}

void tennis_scoreboard_face_resign(movement_settings_t *settings, void *context) {
    (void) settings;
    (void) context;

    // handle any cleanup before your watch face goes off-screen.
}

