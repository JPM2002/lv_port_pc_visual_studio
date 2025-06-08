#include "audio_metadata.h"
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <cstring>  // for strncpy

bool audio_get_metadata(const char* filePath, AudioMetadata* metadata)
{
    TagLib::FileRef f(filePath);
    if (f.isNull() || !f.tag())
        return false;

    TagLib::Tag* tag = f.tag();

    strncpy(metadata->title, tag->title().toCString(true), sizeof(metadata->title));
    strncpy(metadata->artist, tag->artist().toCString(true), sizeof(metadata->artist));
    strncpy(metadata->album, tag->album().toCString(true), sizeof(metadata->album));
    strncpy(metadata->genre, tag->genre().toCString(true), sizeof(metadata->genre));
    metadata->year = tag->year();
    metadata->track = tag->track();

    return true;
}
