#include "audio_metadata.h"
#include <cstdio> // For printf

int main()
{
    AudioMetadata meta;
    if (audio_get_metadata("C:\\Users\\Javie\\Music\\All My Music\\12 gloria.mp3", &meta))
    {
        printf("Title: %s\n", meta.title);
        printf("Artist: %s\n", meta.artist);
        printf("Album: %s\n", meta.album);
        printf("Year: %u\n", meta.year);
        printf("Genre: %s\n", meta.genre);
        printf("Track: %u\n", meta.track);
    }
    else
    {
        printf("Failed to read metadata.\n");
    }

    return 0;
}
