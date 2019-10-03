#include <stddef.h>
#include <stdint.h>
#include "midi.h"
#include "uni_list.h"
#include "heap.h"
#include "beeper.h"
#include "timers.h"
#include "clib/stdio.h"
#include "clock.h"

// Lista nut
UNI_LIST_C(notes, NOTE)

// Aktualnie grana muzyka
volatile MUSIC *current_music = NULL;

// Inicjuje strukture muzyki
void music_init(MUSIC *music, int speed)
{
    music->current_note = NULL;
    music->speed = speed;
    music->notes = list_notes_create();
}

// Zwalnia pamięć potrzebną na muzyke
void music_free(MUSIC *music)
{
    list_notes_destroy(music->notes);
}

// Dodaje note na koniec muzyki
void music_add(MUSIC *music, int32_t frequency, int32_t len)
{
    NOTE note = {frequency, len};
    list_notes_push_back(music->notes, note);
}

// Uruchamia muzyke
void midi_play(MUSIC *music)
{
    current_music = music;
    current_music->current_note = NULL;
    play_next_note();
}

// Gra pojedyńczą nutę
void play_next_note(void)
{
    // Jeżeli muzyka nie jest jeszcze włączona to rozpoczyna od pierwszej nuty
    if(current_music->current_note==NULL)
        current_music->current_note = current_music->notes->head;

    uint32_t time = (uint32_t)current_music->current_note->data.time * current_music->speed;
    uint32_t frequency = current_music->current_note->data.frequency;

    // Uruchamia dzwięk
    beep_start(frequency);
    current_music->current_note = current_music->current_note->next;

    // To nie jest ostatnia nuta
    if(current_music->current_note!=NULL)
        timer_add(play_next_note, time, 0);

    // To jest ostatnia nuta
    else
    {
        timer_add(silent, time, 0);
        current_music = NULL;
    }
}


