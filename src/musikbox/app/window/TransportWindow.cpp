//////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2007-2016 musikcube team
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

#include "stdafx.h"
#include "TransportWindow.h"

#include <cursespp/Screen.h>
#include <cursespp/Colors.h>
#include <cursespp/Text.h>

#include <app/util/Duration.h>
#include <app/util/Playback.h>

#include <core/debug.h>
#include <core/library/LocalLibraryConstants.h>
#include <core/runtime/Message.h>

#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/chrono.hpp>
#include <boost/lexical_cast.hpp>

#include <algorithm>
#include <memory>
#include <deque>

using namespace musik::core;
using namespace musik::core::audio;
using namespace musik::core::library;
using namespace musik::core::db;
using namespace musik::core::sdk;
using namespace musik::core::runtime;
using namespace musik::box;
using namespace boost::chrono;
using namespace cursespp;

#define REFRESH_TRANSPORT_READOUT 1001
#define REFRESH_INTERVAL_MS 1000
#define DEFAULT_TIME -1.0f
#define TIME_SLOP 3.0f

#define MIN_WIDTH 20
#define MIN_HEIGHT 2

#define DEBOUNCE_REFRESH(mode, delay) \
    this->RemoveMessage(REFRESH_TRANSPORT_READOUT); \
    this->PostMessage(REFRESH_TRANSPORT_READOUT, (int64) mode, 0, delay);

#define ON(w, a) if (a != -1) { wattron(w, a); }
#define OFF(w, a) if (a != -1) { wattroff(w, a); }

static std::string playingFormat = "playing $title by $artist from $album";

struct Token {
    enum Type { Normal, Placeholder };

    static std::unique_ptr<Token> New(const std::string& value, Type type) {
        return std::unique_ptr<Token>(new Token(value, type));
    }

    Token(const std::string& value, Type type) {
        this->value = value;
        this->type = type;
    }

    std::string value;
    Type type;
};

typedef std::unique_ptr<Token> TokenPtr;
typedef std::vector<TokenPtr> TokenList;

/* tokenizes an input string that has $placeholder values */
void tokenize(const std::string& format, TokenList& tokens) {
    tokens.clear();
    Token::Type type = Token::Normal;
    size_t i = 0;
    size_t start = 0;
    while (i < format.size()) {
        char c = format[i];
        if ((type == Token::Placeholder && c == ' ') ||
            (type == Token::Normal && c == '$')) {
            /* escape $ with $$ */
            if (c == '$' && i < format.size() - 1 && format[i + 1] == '$') {
                i++;
            }
            else {
                if (i > start) {
                    tokens.push_back(Token::New(format.substr(start, i - start), type));
                }
                start = i;
                type = (c == ' ')  ? Token::Normal : Token::Placeholder;
            }
        }
        ++i;
    }

    if (i > 0) {
        tokens.push_back(Token::New(format.substr(start, i - start), type));
    }
}

/* writes the colorized formatted string to the specified window. accounts for
utf8 characters and ellipsizing */
static size_t writePlayingFormat(
    WINDOW *w,
    std::string title,
    std::string album,
    std::string artist,
    size_t width)
{
    TokenList tokens;
    tokenize(playingFormat, tokens);

    int64 dim = COLOR_PAIR(CURSESPP_TEXT_DISABLED);
    int64 gb = COLOR_PAIR(CURSESPP_TEXT_ACTIVE);
    size_t remaining = width;

    auto it = tokens.begin();
    while (it != tokens.end() && remaining > 0) {
        Token *token = it->get();

        int64 attr = dim;
        std::string value;

        if (token->type == Token::Placeholder) {
            attr = gb;
            if (token->value == "$title") {
                value = title;
            }
            else if (token->value == "$album") {
                value = album;
            }
            else if (token->value == "$artist") {
                value = artist;
            }
        }

        if (!value.size()) {
            value = token->value;
        }

        size_t len = u8cols(value);
        bool ellipsized = false;

        if (len > remaining) {
            std::string original = value;
            value = text::Ellipsize(value, remaining);
            ellipsized = (value != original);
            len = remaining;
        }

        /* if we're not at the last token, but there's not enough space
        to show the next token, ellipsize now and bail out of the loop */
        if (remaining - len < 3 && it + 1 != tokens.end()) {
            if (!ellipsized) {
                value = text::Ellipsize(value, remaining - 3);
                len = remaining;
            }
        }

        ON(w, attr);
        wprintw(w, value.c_str());
        OFF(w, attr);

        remaining -= len;
        ++it;
    }

    return (width - remaining);
}

static inline bool inc(const std::string& kn) {
    return (/*kn == "KEY_UP" ||*/ kn == "KEY_RIGHT");
}

static inline bool dec(const std::string& kn) {
    return (/*kn == "KEY_DOWN" ||*/ kn == "KEY_LEFT");
}

TransportWindow::TransportWindow(musik::box::PlaybackService& playback)
: Window(nullptr)
, playback(playback)
, transport(playback.GetTransport())
, focus(FocusNone) {
    this->SetFrameVisible(false);
    this->playback.TrackChanged.connect(this, &TransportWindow::OnPlaybackServiceTrackChanged);
    this->playback.ModeChanged.connect(this, &TransportWindow::OnPlaybackModeChanged);
    this->playback.Shuffled.connect(this, &TransportWindow::OnPlaybackShuffled);
    this->transport.VolumeChanged.connect(this, &TransportWindow::OnTransportVolumeChanged);
    this->transport.TimeChanged.connect(this, &TransportWindow::OnTransportTimeChanged);
    this->paused = false;
    this->lastTime = DEFAULT_TIME;
}

TransportWindow::~TransportWindow() {
}

void TransportWindow::SetFocus(FocusTarget target) {
    if (target != this->focus) {
        auto last = this->focus;
        this->focus = target;

        if (this->focus == FocusNone) {
            this->lastFocus = last;
            this->Blur();
        }
        else {
            this->Focus();
        }

        DEBOUNCE_REFRESH(TimeSync, 0);
    }
}

TransportWindow::FocusTarget TransportWindow::GetFocus() const {
    return this->focus;
}

bool TransportWindow::KeyPress(const std::string& kn) {
    if (this->focus == FocusVolume) {
        if (inc(kn)) {
            playback::VolumeUp(this->transport);
            return true;
        }
        else if (dec(kn)) {
            playback::VolumeDown(this->transport);
            return true;
        }
        else if (kn == "KEY_ENTER") {
            transport.SetMuted(!transport.IsMuted());
            return true;
        }
    }
    else if (this->focus == FocusTime) {
        if (inc(kn)) {
            playback::SeekForward(this->transport);
            return true;
        }
        else if (dec(kn)) {
            playback::SeekBack(this->transport);
            return true;
        }
    }

    return false;
}

bool TransportWindow::FocusNext() {
    this->SetFocus((FocusTarget)(((int) this->focus + 1) % 3));
    return (this->focus != FocusNone);
}

bool TransportWindow::FocusPrev() {
    this->SetFocus((FocusTarget)(((int) this->focus - 1) % 3));
    return (this->focus != FocusNone);
}

void TransportWindow::FocusFirst() {
    this->SetFocus(FocusVolume);
}

void TransportWindow::FocusLast() {
    this->SetFocus(FocusTime);
}

void TransportWindow::RestoreFocus() {
    this->Focus();
    this->SetFocus(this->lastFocus);
}

void TransportWindow::OnFocusChanged(bool focused) {
    if (!focused) {
        this->SetFocus(FocusNone);
    }
}

void TransportWindow::ProcessMessage(IMessage &message) {
    int type = message.Type();

    if (type == REFRESH_TRANSPORT_READOUT) {
        this->Update((TimeMode) message.UserData1());

        if (transport.GetPlaybackState() != PlaybackStopped) {
            DEBOUNCE_REFRESH(TimeSmooth, REFRESH_INTERVAL_MS)
        }
    }
}

void TransportWindow::OnPlaybackServiceTrackChanged(size_t index, TrackPtr track) {
    this->currentTrack = track;
    this->lastTime = DEFAULT_TIME;
    DEBOUNCE_REFRESH(TimeSync, 0);
}

void TransportWindow::OnPlaybackModeChanged() {
    DEBOUNCE_REFRESH(TimeSync, 0);
}

void TransportWindow::OnTransportVolumeChanged() {
    DEBOUNCE_REFRESH(TimeSync, 0);
}

void TransportWindow::OnTransportTimeChanged(double time) {
    DEBOUNCE_REFRESH(TimeSmooth, 0);
}

void TransportWindow::OnPlaybackShuffled(bool shuffled) {
    DEBOUNCE_REFRESH(TimeSync, 0);
}

void TransportWindow::OnRedraw() {
    this->Update();
}

void TransportWindow::Update(TimeMode timeMode) {
    this->Clear();

    size_t cx = (size_t) this->GetContentWidth();

    if (cx < MIN_WIDTH || this->GetContentHeight() < MIN_HEIGHT) {
        return;
    }

    WINDOW *c = this->GetContent();
    bool paused = (transport.GetPlaybackState() == PlaybackPaused);
    bool stopped = (transport.GetPlaybackState() == PlaybackStopped);
    bool muted = transport.IsMuted();

    int64 gb = COLOR_PAIR(CURSESPP_TEXT_ACTIVE);
    int64 disabled = COLOR_PAIR(CURSESPP_TEXT_DISABLED);

    int64 volumeAttrs = -1;
    if (this->focus == FocusVolume) {
        volumeAttrs = COLOR_PAIR(CURSESPP_TEXT_FOCUSED);
    }
    else if (muted) {
        volumeAttrs = gb;
    }

    int64 timerAttrs = (this->focus == FocusTime)
        ? COLOR_PAIR(CURSESPP_TEXT_FOCUSED) : -1;

    /* prepare the "shuffle" label */

    std::string shuffleLabel = "  shuffle";
    size_t shuffleLabelLen = u8cols(shuffleLabel);

    /* playing SONG TITLE from ALBUM NAME */

    int secondsTotal = 0;

    if (stopped) {
        ON(c, disabled);
        wprintw(c, "playback is stopped");
        OFF(c, disabled);
    }
    else {
        secondsTotal = (int) transport.GetDuration();

        std::string title, album, artist;

        if (this->currentTrack) {
            title = this->currentTrack->GetValue(constants::Track::TITLE);
            album = this->currentTrack->GetValue(constants::Track::ALBUM);
            artist = this->currentTrack->GetValue(constants::Track::ARTIST);

            if (secondsTotal <= 0) {
                std::string duration =
                    this->currentTrack->GetValue(constants::Track::DURATION);

                if (duration.size()) {
                    secondsTotal = boost::lexical_cast<int>(duration);
                }
            }
        }

        title = title.size() ? title : "[song]";
        album = album.size() ? album : "[album]";
        artist = artist.size() ? artist : "[artist]";

        writePlayingFormat(
            c,
            title,
            album,
            artist,
            cx - shuffleLabelLen);
    }

    wmove(c, 0, cx - shuffleLabelLen);
    int64 shuffleAttrs = this->playback.IsShuffled() ? gb : disabled;
    ON(c, shuffleAttrs);
    wprintw(c, shuffleLabel.c_str());
    OFF(c, shuffleAttrs);

    /* volume slider */

    int volumePercent = (size_t) round(this->transport.Volume() * 100.0f) - 1;
    int thumbOffset = std::min(9, (volumePercent * 10) / 100);

    std::string volume;

    if (muted) {
        volume = "muted  ";
    }
    else {
        volume = "vol ";

        for (int i = 0; i < 10; i++) {
            volume += (i == thumbOffset) ? "■" : "─";
        }

        volume += boost::str(boost::format(
            " %d") % (int)std::round(this->transport.Volume() * 100));

        volume += "%%  ";
    }

    /* repeat mode setup */

    RepeatMode mode = this->playback.GetRepeatMode();
    std::string repeatModeLabel = "  repeat ";
    int64 repeatAttrs = -1;
    switch (mode) {
        case RepeatList:
            repeatModeLabel += "list";
            repeatAttrs = gb;
            break;
        case RepeatTrack:
            repeatModeLabel += "track";
            repeatAttrs = gb;
            break;
        default:
            repeatModeLabel += "off";
            repeatAttrs = disabled;
            break;
    }

    /* time slider */

    int64 currentTimeAttrs = timerAttrs;

    if (paused) { /* blink the track if paused */
        int64 now = duration_cast<seconds>(
            system_clock::now().time_since_epoch()).count();

        if (now % 2 == 0) {
            currentTimeAttrs = COLOR_PAIR(CURSESPP_TEXT_HIDDEN);
        }
    }

    /* calculating playback time is inexact because it's based on buffers that
    are sent to the output. here we use a simple smoothing function to hopefully
    mitigate jumping around. basically: draw the time as one second more than the
    last time we displayed, unless they are more than few seconds apart. note this
    only works if REFRESH_INTERVAL_MS is 1000. */
    int secondsCurrent = (int) round(this->lastTime); /* mode == TimeLast */

    if (timeMode == TimeSmooth) {
        double smoothedTime = this->lastTime += 1.0f; /* 1000 millis */
        double actualTime = transport.Position();

        if (paused || stopped || fabs(smoothedTime - actualTime) > TIME_SLOP) {
            smoothedTime = actualTime;
        }

        this->lastTime = smoothedTime;
        /* end time smoothing */

        secondsCurrent = (int) round(smoothedTime);
    }
    else if (timeMode == TimeSync) {
        this->lastTime = transport.Position();
        secondsCurrent = (int) round(this->lastTime);
    }

    const std::string currentTime = duration::Duration(std::min(secondsCurrent, secondsTotal));
    const std::string totalTime = duration::Duration(secondsTotal);

    int bottomRowControlsWidth =
        u8cols(volume) - (muted ? 0 : 1) + /* -1 for escaped percent sign when not muted */
        u8cols(currentTime) + 1 + /* +1 for space padding */
        /* timer track with thumb */
        1 + u8cols(totalTime) + /* +1 for space padding */
        u8cols(repeatModeLabel);

    int timerTrackWidth =
        this->GetContentWidth() -
        bottomRowControlsWidth;

    thumbOffset = 0;

    if (secondsTotal) {
        int progress = (secondsCurrent * 100) / secondsTotal;
        thumbOffset = std::min(timerTrackWidth - 1, (progress * timerTrackWidth) / 100);
    }

    std::string timerTrack = "";
    for (int i = 0; i < timerTrackWidth; i++) {
        timerTrack += (i == thumbOffset) ? "■" : "─";
    }

    /* draw second row */

    wmove(c, 1, 0); /* move cursor to the second line */

    ON(c, volumeAttrs);
    wprintw(c, volume.c_str());
    OFF(c, volumeAttrs);

    ON(c, currentTimeAttrs); /* blink if paused */
    wprintw(c, "%s ", currentTime.c_str());
    OFF(c, currentTimeAttrs);

    ON(c, timerAttrs);
    waddstr(c, timerTrack.c_str()); /* may be a very long string */
    wprintw(c, " %s", totalTime.c_str());
    OFF(c, timerAttrs);

    ON(c, repeatAttrs);
    wprintw(c, repeatModeLabel.c_str());
    OFF(c, repeatAttrs);

    this->Invalidate();
}
