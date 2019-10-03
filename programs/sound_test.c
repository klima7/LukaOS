#include <stddef.h>
#include <stdint.h>
#include "sound_test.h"
#include "../clib/stdio.h"
#include "../midi.h"
#include "../clock.h"

int sound_test_main(const char* argv, uint32_t argc)
{
    printf("Happy Birthday!\n");

    MUSIC music = {0};
    music_init(&music, 100);

    music_add(&music, G1, LEN4);
    music_add(&music, E1, LEN4);
    music_add(&music, PAUSE, LEN4);
    music_add(&music, G1, LEN4);
    music_add(&music, E1, LEN4);
    music_add(&music, PAUSE, LEN4);
    music_add(&music, G1, LEN4);
    music_add(&music, A1, LEN8);
    music_add(&music, G1, LEN8);
    music_add(&music, F1, LEN8);
    music_add(&music, E1, LEN8);
    music_add(&music, F1, LEN4);

    midi_play(&music);

    // Czeka 10 sekund aż muzyka się skończy
    sleep(10000);
    music_free(&music);

    return 0;
}