// SPDX-License-Identifier: GPL-3.0-or-later
//
// Copyright (C) 2015, 2020 Antonio Ni�o D�az
//
// Pong 3DS. Just a pong for the Nintendo 3DS.

#include <stdlib.h>
#include <string.h>

#include <3ds.h>

#include "S3D/engine.h"

#include "ball.h"
#include "game.h"
#include "pad.h"
#include "rooms.h"
#include "screens_2d.h"
#include "sound.h"
#include "utils.h"

// -----------------------------------------------------------------------------

static int game_paused = 0;

void Game_Pause(int pause)
{
    game_paused = pause;
}

int Game_IsPaused(void)
{
    return game_paused;
}

// -----------------------------------------------------------------------------

static _game_state_e game_state_machine;
static int game_state_machine_delay; // Pause game during this number of frames

void Game_StateMachineReset(void)
{
    Game_Pause(0);
    game_state_machine = GAME_STARTING;
    game_state_machine_delay = 120;
}

void Game_PlayerScoreStartDelay(void)
{
    if (game_state_machine != GAME_NORMAL_PLAY)
        return;

    game_state_machine = GAME_GOAL_DELAY;
    game_state_machine_delay = 30; // 30 frames
}

int Game_StateMachinePadMovementEnabled(void)
{
    switch (game_state_machine)
    {
        case GAME_INITIAL_DELAY:
        case GAME_NORMAL_PLAY:
            return 1;

        case GAME_STARTING:
        case GAME_ENDING:
        case GAME_GOAL_DELAY:
            return 0;

        default:
            return 0;
    }
}

int Game_StateMachineBallMovementEnabled(void)
{
    switch (game_state_machine)
    {
        case GAME_STARTING:
        case GAME_ENDING:
        case GAME_INITIAL_DELAY:
            return 0;

        case GAME_NORMAL_PLAY:
        case GAME_GOAL_DELAY:
            return 1;

        default:
            return 0;
    }
}

int Game_StateMachineBallAddScoreEnabled(void)
{
    switch (game_state_machine)
    {
        case GAME_INITIAL_DELAY:
        case GAME_STARTING:
        case GAME_ENDING:
        case GAME_GOAL_DELAY:
            return 0;

        case GAME_NORMAL_PLAY:
            return 1;

        default:
            return 0;
    }
}

void Game_UpdateStateMachine(void)
{
    int need_to_change = 0;

    if (game_state_machine_delay)
    {
        game_state_machine_delay--;
        if (game_state_machine_delay == 0)
        {
            need_to_change = 1;
        }
    }

    if (need_to_change == 0)
        return;

    switch (game_state_machine)
    {
        case GAME_STARTING:
            game_state_machine = GAME_INITIAL_DELAY;
            game_state_machine_delay = 30;
            Ball_Reset();
            Pad_ResetAll();
            return;
        case GAME_INITIAL_DELAY:
            game_state_machine = GAME_NORMAL_PLAY;
            return;
        case GAME_NORMAL_PLAY:
            return;
        case GAME_GOAL_DELAY:
            if (Game_PlayerScoreGet(0) >= GAME_WIN_SCORE)
            {
                game_state_machine = GAME_ENDING;
                game_state_machine_delay = 300;
            }
            else if (Game_PlayerScoreGet(1) >= GAME_WIN_SCORE)
            {
                game_state_machine = GAME_ENDING;
                game_state_machine_delay = 300;
            }
            else
            {
                game_state_machine = GAME_INITIAL_DELAY;
                game_state_machine_delay = 30;
                Ball_Reset();
                Pad_ResetAll();
            }
            return;
        case GAME_ENDING:
            Room_SetNumber(GAME_ROOM_MENU);
            return;

        default:
            return;
    }
}

_game_state_e Game_StateMachineGet(void)
{
    return game_state_machine;
}

// -----------------------------------------------------------------------------

typedef struct
{
    int score;
} _player_info_s;

_player_info_s PLAYER[2];

void Game_PlayerResetAll(void)
{
    memset(&PLAYER, 0, sizeof(PLAYER));
}

void Game_PlayerScoreIncrease(int player)
{
    PLAYER[player].score++;
}

int Game_PlayerScoreGet(int player)
{
    return PLAYER[player].score;
}

// -----------------------------------------------------------------------------

struct
{
    int r, g, b;
    int vr, vg, vb;
} _clear_color;

void ClearColorInit(void)
{
    _clear_color.r = _clear_color.g = _clear_color.b = 64;
    _clear_color.vr = (fast_rand() & 3) + 1;
    _clear_color.vg = (fast_rand() & 3) + 1;
    _clear_color.vb = (fast_rand() & 3) + 1;

    if (fast_rand() & 1)
        _clear_color.vr = -_clear_color.vr;
    if (fast_rand() & 1)
        _clear_color.vg = -_clear_color.vg;
    if (fast_rand() & 1)
        _clear_color.vb = -_clear_color.vb;
}

void ClearColorHandle(void)
{
    if (Game_IsPaused())
        return;

    _clear_color.r += _clear_color.vr;
    _clear_color.g += _clear_color.vg;
    _clear_color.b += _clear_color.vb;

    if (_clear_color.r > 128)
    {
        _clear_color.r = 128;
        _clear_color.vr = -_clear_color.vr;
    }
    else if (_clear_color.r < 0)
    {
        _clear_color.r = 0;
        _clear_color.vr = -_clear_color.vr;
    }

    if (_clear_color.g > 128)
    {
        _clear_color.g = 128;
        _clear_color.vg = -_clear_color.vg;
    }
    else if (_clear_color.g < 0)
    {
        _clear_color.g = 0;
        _clear_color.vg = -_clear_color.vg;
    }

    if (_clear_color.b > 128)
    {
        _clear_color.b = 128;
        _clear_color.vb = -_clear_color.vb;
    }
    else if (_clear_color.b < 0)
    {
        _clear_color.b = 0;
        _clear_color.vb = -_clear_color.vb;
    }
}

// -----------------------------------------------------------------------------

void Game_DrawScreenTop(int screen)
{
    S3D_FramebuffersClearTopScreen(screen, _clear_color.r, _clear_color.g,
                                   _clear_color.b);

    // 3D stuff
    Room_Draw(screen);

    S3D_PolygonListFlush(screen, 1);

    // 2D stuff
    Draw2D_TopScreen(screen);
}

void Game_DrawScreenBottom(void)
{
    Draw2D_BottomScreen();
}

// -----------------------------------------------------------------------------

#include "bounce_raw_bin.h"
#include "jump_raw_bin.h"
#include "select_raw_bin.h"

void Game_Init(void)
{
    Sound_LoadSfx(SFX_BOUNCE_REF, bounce_raw_bin, bounce_raw_bin_size);
    Sound_LoadSfx(SFX_JUMP_REF, jump_raw_bin, jump_raw_bin_size);
    Sound_LoadSfx(SFX_SELECT_REF, select_raw_bin, select_raw_bin_size);

    ClearColorInit();

    Room_SetNumber(GAME_ROOM_MENU);
}

void Game_Handle(void)
{
    ClearColorHandle();

    Room_Handle();
}

void Game_End(void)
{
    // Nothing here...
}
