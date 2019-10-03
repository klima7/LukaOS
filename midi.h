#ifndef __MIDI_H__
#define __MIDI_H__

#include <stddef.h>
#include <stdint.h>
#include "uni_list.h"
#include "heap.h"

// Pierwsza oktawa
#define C1      262
#define CIS1    277
#define D1      294
#define DIS1    311
#define E1      330
#define F1      349
#define FIS1    370
#define G1      392
#define GIS1    415
#define A1      440
#define H1      494

// Druga oktawa
#define C2      523
#define CIS2    554
#define D2      587
#define DIS2    622
#define E2      659
#define F2      698
#define FIS2    740
#define G2      784
#define GIS2    830
#define A2      880
#define H2      988

// Pauza
#define PAUSE   0

// Długości dzwięków
#define LEN1    32      // Cała nuta
#define LEN2    16      // Półnuta
#define LEN4    8       // Ćwierćnuta
#define LEN8    4       // Ósemka
#define LEN16   2       // Szesnastka
#define LEN32   1       // Trzydziestka dwójka

// Struktura opisująca nutę
struct note_t
{
    int32_t frequency;
    int32_t time;
};

// Struktura opisująca muzykę
struct music_t
{
    struct list_notes_t *notes;
    struct node_notes_t *current_note;
    int speed;
};

// Typedefy
typedef struct note_t NOTE;
typedef struct music_t MUSIC;

// Lista nut
UNI_LIST_H(notes, NOTE)

// Prototypy
void midi_play(MUSIC *music);
void music_init(MUSIC *music, int speed);
void music_free(MUSIC *music);
void music_add(MUSIC *music, int32_t frequency, int32_t len);
void play_next_note(void);

#endif