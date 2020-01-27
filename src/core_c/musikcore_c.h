//////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2004-2019 musikcube team
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright notice,
//      this list of conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//
//    * Neither the name of the author nor the names of other contributors may
//      be used to endorse or promote products derived from this software
//      without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __C_MUSIKCORE_SDK_C_H__
#define __C_MUSIKCORE_SDK_C_H__

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#ifdef DEFINE_EXPORTS
    #ifdef WIN32
        #define mcsdk_export extern "C" __declspec(dllexport)
    #else
        #define mcsdk_export extern "C"
    #endif
#else
    #define mcsdk_export extern
#endif

/*
 *
 * version
 *
 */

static const int mcsdk_version = 18;

/*
 *
 * constants
 *
 */

typedef enum mcsdk_playback_state {
    mcsdk_playback_stopped = 1,
    mcsdk_playback_paused = 2,
    mcsdk_playback_prepared = 3,
    mcsdk_playback_playing = 4,
} mcsdk_playback_state;

typedef enum mcsdk_stream_event {
    mcsdk_stream_scheduled = 1,
    mcsdk_stream_prepared = 2,
    mcsdk_stream_playing = 3,
    mcsdk_stream_almost_done = 4,
    mcsdk_stream_finished = 5,
    mcsdk_stream_stopped = 6,
    mcsdk_stream_error = -1
} mcsdk_stream_event;

typedef enum mcsdk_repeat_mode {
    mcsdk_repeat_none = 0,
    mcsdk_repeat_track = 1,
    mcsdk_repeat_list = 2
} mcsdk_repeat_mode;

typedef enum mcsdk_output_code {
    mcsdk_output_error_invalid_format = -4,
    mcsdk_output_error_invalid_state = -3,
    mcsdk_output_error_buffer_full = -2,
    mcsdk_output_error_buffer_written = -1
} mcsdk_output_code;

typedef enum mcsdk_time_change_mode {
    mcsdk_time_change_mode_seek = 0,
    mcsdk_time_change_mode_scrub = 1
} mcsdk_time_change_mode;

typedef enum mcsdk_path_type {
    mcsdk_path_user_home = 0,
    mcsdk_path_data = 1,
    mcsdk_path_application = 2,
    mcsdk_path_plugins = 3,
    mcsdk_path_library = 4
} mcsdk_path_type;

typedef enum mcsdk_stream_capability {
    mcsdk_stream_capability_prebuffer = 0x01
} mcsdk_stream_capability;

typedef enum mcsdk_indexer_scan_result {
    mcsdk_indexer_scan_result_commit = 1,
    mcsdk_indexer_scan_result_rollback = 2
} mcsdk_indexer_scan_result;

typedef enum mcsdk_replay_gain_mode {
    mcsdk_replay_gain_mode_disabled = 0,
    mcsdk_replay_gain_mode_track = 1,
    mcsdk_replay_gain_mode_album = 2
} mcsdk_replay_gain_mode;

typedef enum mcsdk_transport_type {
    mcsdk_transport_type_gapless = 0,
    mcsdk_transport_type_crossfade = 1
} mcsdk_transport_type;

typedef enum mcsdk_stream_open_flags {
    mcsdk_stream_open_flags_none = 0,
    mcsdk_stream_open_flags_read = 1,
    mcsdk_stream_open_flags_write = 2
} mcsdk_stream_open_flags;

typedef enum mcsdk_resource_class {
    mcsdk_resource_type_value = 0,
    mcsdk_resource_type_map = 1
} mcsdk_resource_class;

typedef enum mcsdk_encoder_type {
    mcsdk_encoder_type_blocking = 0,
    mcsdk_encoder_type_streaming = 1
} mcsdk_encoder_type;

static const size_t mcsdk_equalizer_band_count = 18;

static const size_t mcsdk_equalizer_bands[] = {
    65, 92, 131, 185, 262, 370, 523, 740, 1047, 1480,
    2093, 2960, 4186, 5920, 8372, 11840, 16744, 22000
};

static const int mcsdk_no_offset = 0;
static const int mcsdk_no_limit = -1;

static const char* mcsdk_category_type_album = "album";
static const char* mcsdk_category_type_artist = "artist";
static const char* mcsdk_category_type_album_artist = "album_artist";
static const char* mcsdk_category_type_genre = "genre";
static const char* mcsdk_category_type_playlist = "playlists";

static const char* mcsdk_track_field_id = "id";
static const char* mcsdk_track_field_track_num = "track";
static const char* mcsdk_track_field_disc_num = "disc";
static const char* mcsdk_track_field_bpm = "bpm";
static const char* mcsdk_track_field_duration = "duration";
static const char* mcsdk_track_field_filesize = "filesize";
static const char* mcsdk_track_field_year = "year";
static const char* mcsdk_track_field_title = "title";
static const char* mcsdk_track_field_filename = "filename";
static const char* mcsdk_track_field_thumbnail_id = "thumbnail_id";
static const char* mcsdk_track_field_album = "album";
static const char* mcsdk_track_field_album_artist = "album_artist";
static const char* mcsdk_track_field_genre = "genre";
static const char* mcsdk_track_field_artist = "artist";
static const char* mcsdk_track_field_filetime = "filetime";
static const char* mcsdk_track_field_genre_id = "visual_genre_id";
static const char* mcsdk_track_field_artist_id = "visual_artist_id";
static const char* mcsdk_track_field_album_artist_id = "album_artist_id";
static const char* mcsdk_track_field_album_id = "album_id";
static const char* mcsdk_track_field_source_id = "source_id";
static const char* mcsdk_track_field_external_id = "external_id";

/*
 *
 * types
 *
 */

#define DEFINE_HANDLE(x) \
    typedef struct x { \
        void* opaque; \
    } x;

DEFINE_HANDLE(mcsdk_internal);
DEFINE_HANDLE(mcsdk_resource);
DEFINE_HANDLE(mcsdk_value);
DEFINE_HANDLE(mcsdk_value_list);
DEFINE_HANDLE(mcsdk_map);
DEFINE_HANDLE(mcsdk_track);
DEFINE_HANDLE(mcsdk_map_list);
DEFINE_HANDLE(mcsdk_track_list);
DEFINE_HANDLE(mcsdk_track_list_editor);
DEFINE_HANDLE(mcsdk_svc_metadata);
DEFINE_HANDLE(mcsdk_svc_playback);
DEFINE_HANDLE(mcsdk_prefs);
DEFINE_HANDLE(mcsdk_audio_buffer);
DEFINE_HANDLE(mcsdk_audio_buffer_provider);
DEFINE_HANDLE(mcsdk_data_stream);
DEFINE_HANDLE(mcsdk_device);
DEFINE_HANDLE(mcsdk_device_list);
DEFINE_HANDLE(mcsdk_output);
DEFINE_HANDLE(mcsdk_decoder);
DEFINE_HANDLE(mcsdk_encoder);
DEFINE_HANDLE(mcsdk_blocking_encoder);
DEFINE_HANDLE(mcsdk_streaming_encoder);

/*
 *
 * instance context
 *
 */

typedef struct mcsdk_context {
    mcsdk_svc_metadata metadata;
    mcsdk_svc_playback playback;
    mcsdk_prefs preferences;
    mcsdk_internal internal;
} mcsdk_context;

mcsdk_export void mcsdk_context_init(mcsdk_context** context);
mcsdk_export void mcsdk_context_release(mcsdk_context** context);
mcsdk_export void mcsdk_set_plugin_context(mcsdk_context* context);
mcsdk_export bool mcsdk_is_plugin_context(mcsdk_context* context);

/*
 *
 * IResource
 *
 */

mcsdk_export int64_t mcsdk_resource_get_id(mcsdk_resource r);
mcsdk_export mcsdk_resource_class mcsdk_resource_get_class(mcsdk_resource r);
mcsdk_export void mcsdk_resource_release(mcsdk_resource r);

/*
 *
 * IValue
 *
 */

mcsdk_export size_t mcsdk_value_get_value(mcsdk_value v, char* dst, size_t size);
mcsdk_export void mcsdk_value_release(mcsdk_value v);

/*
 *
 * IValueList
 *
 */

mcsdk_export size_t mcsdk_value_list_count(mcsdk_value_list vl);
mcsdk_export mcsdk_value mcsdk_value_list_get_at(mcsdk_value_list vl, size_t index);
mcsdk_export void mcsdk_value_list_release(mcsdk_value_list vl);

/*
 *
 * IMap
 *
 */

mcsdk_export int mcsdk_map_get_string(mcsdk_map m, const char* key, char* dst, int size);
mcsdk_export int64_t mcsdk_map_get_int64(mcsdk_map m, const char* key, int64_t default_value);
mcsdk_export int32_t mcsdk_map_get_int32(mcsdk_map m, const char* key, int32_t default_value);
mcsdk_export double mcsdk_map_get_double(mcsdk_map m, const char* key, double default_value);
mcsdk_export void mcsdk_map_release(mcsdk_map m);

/*
 *
 * IMapList
 *
 */

mcsdk_export size_t mcsdk_map_list_get_count(mcsdk_map_list ml);
mcsdk_export mcsdk_map mcsdk_map_list_get_at(mcsdk_map_list ml, size_t index);
mcsdk_export void mcsdk_map_list_release(mcsdk_map_list ml);

/*
 *
 * ITrack
 *
 */

mcsdk_export void mcsdk_track_retain(mcsdk_track t);
mcsdk_export int mcsdk_track_get_uri(mcsdk_track t, char* dst, int size);
mcsdk_export void mcsdk_track_release(mcsdk_track t);

/*
 *
 * ITrackList
 *
 */

mcsdk_export size_t mcsdk_track_list_get_count(mcsdk_track_list tl);
mcsdk_export int64_t mcsdk_track_list_get_id(mcsdk_track_list tl, size_t index);
mcsdk_export int64_t mcsdk_track_list_index_of(mcsdk_track_list tl, int64_t id);
mcsdk_export mcsdk_track mcsdk_track_list_get_track_at(mcsdk_track_list tl, size_t index);
mcsdk_export void mcsdk_track_list_release(mcsdk_track_list tl);

/*
 *
 * ITrackListEditor
 *
 */

mcsdk_export bool mcsdk_track_list_editor_insert(mcsdk_track_list_editor tle, int64_t id, size_t index);
mcsdk_export bool mcsdk_track_list_editor_swap(mcsdk_track_list_editor tle, size_t index1, size_t index2);
mcsdk_export bool mcsdk_track_list_editor_move(mcsdk_track_list_editor tle, size_t from, size_t to);
mcsdk_export bool mcsdk_track_list_editor_delete(mcsdk_track_list_editor tle, size_t index);
mcsdk_export void mcsdk_track_list_editor_add(mcsdk_track_list_editor tle, const int64_t id);
mcsdk_export void mcsdk_track_list_editor_clear(mcsdk_track_list_editor tle);
mcsdk_export void mcsdk_track_list_editor_shuffle(mcsdk_track_list_editor tle);
mcsdk_export void mcsdk_track_list_editor_release(mcsdk_track_list_editor tle);

/*
 *
 * IMetadataProxy
 *
 */

mcsdk_export mcsdk_track_list mcsdk_svc_metadata_query_tracks(mcsdk_svc_metadata mp, const char* keyword, int limit, int offset);
mcsdk_export mcsdk_track mcsdk_svc_metadata_query_track_by_id(mcsdk_svc_metadata mp, int64_t track_id);
mcsdk_export mcsdk_track mcsdk_svc_metadata_query_track_by_external_id(mcsdk_svc_metadata mp, const char* external_id);
mcsdk_export mcsdk_track_list mcsdk_svc_metadata_query_tracks_by_category(mcsdk_svc_metadata mp, const char* category_type, int64_t selected_id, const char* filter, int limit, int offset);
mcsdk_export mcsdk_track_list mcsdk_svc_metadata_query_tracks_by_categories(mcsdk_svc_metadata mp, mcsdk_value* categories, size_t category_count, const char* filter, int limit, int offset);
mcsdk_export mcsdk_track_list mcsdk_svc_metadata_query_tracks_by_external_id(mcsdk_svc_metadata mp, const char** external_ids, size_t external_id_count);
mcsdk_export mcsdk_value_list mcsdk_svc_metadata_list_categories(mcsdk_svc_metadata mp);
mcsdk_export mcsdk_value_list mcsdk_svc_metadata_query_category(mcsdk_svc_metadata mp, const char* type, const char* filter);
mcsdk_export mcsdk_value_list mcsdk_svc_metadata_query_category_with_predicate(mcsdk_svc_metadata mp, const char* type, const char* predicate_type, int64_t predicate_id, const char* filter);
mcsdk_export mcsdk_value_list mcsdk_svc_metadata_query_category_with_predicates(mcsdk_svc_metadata mp, const char* type, mcsdk_value* predicates, size_t predicate_count, const char* filter);
mcsdk_export mcsdk_map_list mcsdk_svc_metadata_query_albums(mcsdk_svc_metadata mp, const char* filter);
mcsdk_export mcsdk_map_list mcsdk_svc_metadata_query_albums_by_category(mcsdk_svc_metadata mp, const char* category_id_name, int64_t category_id_value, const char* filter);
mcsdk_export int64_t mcsdk_svc_metadata_save_playlist_with_ids(mcsdk_svc_metadata mp, int64_t* track_ids, size_t track_id_count, const char* playlist_name, const int64_t playlist_id);
mcsdk_export int64_t mcsdk_svc_metadata_save_playlist_with_external_ids(mcsdk_svc_metadata mp, const char** external_ids, size_t external_id_count, const char* playlist_name, const int64_t playlist_id);
mcsdk_export int64_t mcsdk_svc_metadata_save_playlist_with_track_list(mcsdk_svc_metadata mp, mcsdk_track_list track_list, const char* playlist_name, const int64_t playlist_id);
mcsdk_export bool mcsdk_svc_metadata_rename_playlist(mcsdk_svc_metadata mp, const int64_t playlist_id, const char* playlist_name);
mcsdk_export bool mcsdk_svc_metadata_delete_playlist(mcsdk_svc_metadata mp, const int64_t playlist_id);
mcsdk_export bool mcsdk_svc_metadata_append_to_playlist_with_ids(mcsdk_svc_metadata mp, const int64_t playlist_id, const int64_t* track_ids, size_t track_id_count, int offset);
mcsdk_export bool mcsdk_svc_metadata_append_to_playlist_with_external_ids(mcsdk_svc_metadata mp, const int64_t playlist_id, const char** external_track_ids, size_t external_track_id_count, int offset);
mcsdk_export bool mcsdk_svc_metadata_append_to_playlist_with_track_list(mcsdk_svc_metadata mp, const int64_t playlist_id, mcsdk_track_list track_list, int offset);
mcsdk_export size_t mcsdk_svc_metadata_remove_tracks_from_playlist(mcsdk_svc_metadata mp, const int64_t playlist_id, const char** external_ids, const int* sort_orders, int count);
mcsdk_export void mcsdk_svc_metadata_release(mcsdk_svc_metadata mp);

/*
 *
 * IPlaybackService
 *
 */

mcsdk_export void mcsdk_svc_playback_play_at(mcsdk_svc_playback pb, size_t index);
mcsdk_export bool mcsdk_svc_playback_next(mcsdk_svc_playback pb);
mcsdk_export bool mcsdk_svc_playback_previous(mcsdk_svc_playback pb);
mcsdk_export void mcsdk_svc_playback_stop(mcsdk_svc_playback pb);
mcsdk_export mcsdk_repeat_mode mcsdk_svc_playback_get_repeat_mode(mcsdk_svc_playback pb);
mcsdk_export void mcsdk_svc_playback_set_repeat_mode(mcsdk_svc_playback pb, mcsdk_repeat_mode mode);
mcsdk_export void mcsdk_svc_playback_toggle_repeat_mode(mcsdk_svc_playback pb);
mcsdk_export mcsdk_playback_state mcsdk_svc_playback_get_playback_state(mcsdk_svc_playback pb);
mcsdk_export bool mcsdk_svc_playback_is_shuffled(mcsdk_svc_playback pb);
mcsdk_export void mcsdk_svc_playback_toggle_shuffle(mcsdk_svc_playback pb);
mcsdk_export void mcsdk_svc_playback_pause_or_resume(mcsdk_svc_playback pb);
mcsdk_export double mcsdk_svc_playback_get_volume(mcsdk_svc_playback pb);
mcsdk_export void mcsdk_svc_playback_set_volume(mcsdk_svc_playback pb, double volume);
mcsdk_export double mcsdk_svc_playback_get_position(mcsdk_svc_playback pb);
mcsdk_export void mcsdk_svc_playback_set_position(mcsdk_svc_playback pb, double seconds);
mcsdk_export double mcsdk_svc_playback_get_duration(mcsdk_svc_playback pb);
mcsdk_export bool mcsdk_svc_playback_is_muted(mcsdk_svc_playback pb);
mcsdk_export void mcsdk_svc_playback_toggle_mute(mcsdk_svc_playback pb);
mcsdk_export size_t mcsdk_svc_playback_get_index(mcsdk_svc_playback pb);
mcsdk_export size_t mcsdk_svc_playback_count(mcsdk_svc_playback pb);
mcsdk_export mcsdk_track mcsdk_svc_playback_get_track(mcsdk_svc_playback pb, size_t index);
mcsdk_export mcsdk_track mcsdk_svc_playback_get_playing_track(mcsdk_svc_playback pb);
mcsdk_export void mcsdk_svc_playback_copy_from(mcsdk_svc_playback pb, const mcsdk_track_list track_list);
mcsdk_export void mcsdk_svc_playback_play(mcsdk_svc_playback pb, const mcsdk_track_list source, size_t index);
mcsdk_export mcsdk_track_list_editor mcsdk_svc_playback_edit_playlist(mcsdk_svc_playback pb);
mcsdk_export mcsdk_time_change_mode mcsdk_svc_playback_get_time_change_mode(mcsdk_svc_playback pb);
mcsdk_export void mcsdk_svc_playback_set_time_change_mode(mcsdk_svc_playback pb, mcsdk_time_change_mode mode);
mcsdk_export void mcsdk_svc_playback_reload_output(mcsdk_svc_playback pb);
mcsdk_export mcsdk_track_list mcsdk_svc_playback_clone(mcsdk_svc_playback pb);

/*
 *
 * IPreferences
 *
 */

mcsdk_export bool mcsdk_prefs_get_bool(mcsdk_prefs p, const char* key, bool defaultValue);
mcsdk_export int mcsdk_prefs_get_int(mcsdk_prefs p, const char* key, int defaultValue);
mcsdk_export double mcsdk_prefs_get_double(mcsdk_prefs p, const char* key, double defaultValue);
mcsdk_export int mcsdk_prefs_get_string(mcsdk_prefs p, const char* key, char* dst, size_t size, const char* defaultValue);
mcsdk_export void mcsdk_prefs_set_int(mcsdk_prefs p, const char* key, int value);
mcsdk_export void mcsdk_prefs_set_double(mcsdk_prefs p, const char* key, double value);
mcsdk_export void mcsdk_prefs_set_string(mcsdk_prefs p, const char* key, const char* value);
mcsdk_export void mcsdk_prefs_save(mcsdk_prefs p);
mcsdk_export void mcsdk_prefs_release(mcsdk_prefs p);

/*
 *
 * IDataStream
 *
 */

mcsdk_export bool mcsdk_data_stream_open(mcsdk_data_stream ds, const char *uri, mcsdk_stream_open_flags flags);
mcsdk_export bool mcsdk_data_stream_close(mcsdk_data_stream ds);
mcsdk_export void mcsdk_data_stream_interrupt(mcsdk_data_stream ds);
mcsdk_export bool mcsdk_data_stream_is_readable(mcsdk_data_stream ds);
mcsdk_export bool mcsdk_data_stream_is_writable(mcsdk_data_stream ds);
mcsdk_export long mcsdk_data_stream_read(mcsdk_data_stream ds, void* dst, long count);
mcsdk_export long mcsdk_data_stream_write(mcsdk_data_stream ds, void* src, long count);
mcsdk_export bool mcsdk_data_stream_set_position(mcsdk_data_stream ds, long position);
mcsdk_export long mcsdk_data_stream_get_position(mcsdk_data_stream ds);
mcsdk_export bool mcsdk_data_stream_is_seekable(mcsdk_data_stream ds);
mcsdk_export bool mcsdk_data_stream_is_eof(mcsdk_data_stream ds);
mcsdk_export long mcsdk_data_stream_get_length(mcsdk_data_stream ds);
mcsdk_export const char* mcsdk_data_stream_get_type(mcsdk_data_stream ds);
mcsdk_export const char* mcsdk_data_stream_get_uri(mcsdk_data_stream ds);
mcsdk_export bool mcsdk_data_stream_can_prefetch(mcsdk_data_stream ds);
mcsdk_export void mcsdk_data_stream_release(mcsdk_data_stream ds);

/*
 *
 * IBuffer
 *
 */

mcsdk_export long mcsdk_audio_buffer_get_sample_rate(mcsdk_audio_buffer ab);
mcsdk_export void mcsdk_audio_buffer_set_sample_rate(mcsdk_audio_buffer ab, long sample_rate);
mcsdk_export int mcsdk_audio_buffer_get_channels(mcsdk_audio_buffer ab);
mcsdk_export void mcsdk_audio_buffer_set_channels(mcsdk_audio_buffer ab, int channels);
mcsdk_export float* mcsdk_audio_buffer_get_buffer_pointer(mcsdk_audio_buffer ab);
mcsdk_export long mcsdk_audio_buffer_get_sample_count(mcsdk_audio_buffer ab);
mcsdk_export void mcsdk_audio_buffer_set_sample_count(mcsdk_audio_buffer ab, long samples);
mcsdk_export long mcsdk_audio_buffer_get_byte_count(mcsdk_audio_buffer ab);
mcsdk_export void mcsdk_audio_buffer_release(mcsdk_audio_buffer ab);

/*
 *
 * IBufferProvider
 *
 */

mcsdk_export void mcsdk_audio_buffer_provider_notify_processed(mcsdk_audio_buffer_provider abp, mcsdk_audio_buffer ab);

/*
 *
 * IDevice
 *
 */

mcsdk_export const char* mcsdk_device_get_name(mcsdk_device d);
mcsdk_export const char* mcsdk_device_get_id(mcsdk_device d);
mcsdk_export void mcsdk_device_release(mcsdk_device d);

/*
 *
 * IDeviceList
 *
 */

mcsdk_export size_t mcsdk_device_list_get_count(mcsdk_device_list dl);
mcsdk_export const mcsdk_device mcsdk_device_list_get_at(mcsdk_device_list dl, size_t index);
mcsdk_export void mcsdk_device_list_release(mcsdk_device_list dl);

/*
 *
 * IOutput
 *
 */

mcsdk_export void mcsdk_output_pause(mcsdk_output o);
mcsdk_export void mcsdk_output_resume(mcsdk_output o);
mcsdk_export void mcsdk_output_set_volume(mcsdk_output o, double volume);
mcsdk_export double mcsdk_output_get_volume(mcsdk_output o);
mcsdk_export void mcsdk_output_stop(mcsdk_output o);
mcsdk_export int mcsdk_output_play(mcsdk_output o, mcsdk_audio_buffer ab, mcsdk_audio_buffer_provider abp);
mcsdk_export void mcsdk_output_drain(mcsdk_output o);
mcsdk_export double mcsdk_output_get_latency(mcsdk_output o);
mcsdk_export const char* mcsdk_output_get_name(mcsdk_output o);
mcsdk_export mcsdk_device_list mcsdk_output_get_device_list(mcsdk_output o);
mcsdk_export bool mcsdk_output_set_default_device(mcsdk_output o, const char* device_id);
mcsdk_export mcsdk_device mcsdk_output_get_default_device(mcsdk_output o);
mcsdk_export void mcsdk_output_release(mcsdk_output o);

/*
 *
 * IDecoder
 *
 */

mcsdk_export double mcsdk_decoder_set_position(mcsdk_decoder d, double seconds);
mcsdk_export bool mcsdk_decoder_get_buffer(mcsdk_decoder d, mcsdk_audio_buffer ab);
mcsdk_export double mcsdk_decoder_get_duration(mcsdk_decoder d);
mcsdk_export bool mcsdk_decoder_open(mcsdk_decoder d, mcsdk_data_stream ds);
mcsdk_export bool mcsdk_decoder_is_eof(mcsdk_decoder d);
mcsdk_export void mcsdk_decoder_release(mcsdk_decoder d);

/*
 *
 * IEncoder
 *
 */

mcsdk_export mcsdk_encoder_type mcsdk_encoder_get_type(mcsdk_encoder e);
mcsdk_export void mcsdk_encoder_release(mcsdk_encoder e);

/*
 *
 * IBlockingEncoder
 *
 */

mcsdk_export bool mcsdk_blocking_encoder_initialize(mcsdk_blocking_encoder be, mcsdk_data_stream out, size_t rate, size_t channels, size_t bitrate);
mcsdk_export bool mcsdk_blocking_encoder_encode(mcsdk_blocking_encoder be, mcsdk_audio_buffer ab);
mcsdk_export void mcsdk_blocking_encoder_finalize(mcsdk_blocking_encoder be);
mcsdk_export void mcsdk_blocking_encoder_release(mcsdk_blocking_encoder be, mcsdk_encoder e);

/*
 *
 * IStreamingEncoder
 *
 */

mcsdk_export bool mcsdk_streaming_encoder_initialize(mcsdk_streaming_encoder se, size_t rate, size_t channels, size_t bitrate);
mcsdk_export int mcsdk_streaming_encoder_encode(mcsdk_streaming_encoder se, mcsdk_audio_buffer ab, char** data);
mcsdk_export int mcsdk_streaming_encoder_flush(mcsdk_streaming_encoder se, char** data);
mcsdk_export void mcsdk_streaming_encoder_finalize(mcsdk_streaming_encoder se, const char* uri);
mcsdk_export void mcsdk_streaming_encoder_release(mcsdk_streaming_encoder se, mcsdk_encoder e);

/*
 *
 * IDebug
 *
 */

mcsdk_export void mcsdk_debug_verbose(const char* tag, const char* message);
mcsdk_export void mcsdk_debug_info(const char* tag, const char* message);
mcsdk_export void mcsdk_debug_warning(const char* tag, const char* message);
mcsdk_export void mcsdk_debug_error(const char* tag, const char* message);

#endif