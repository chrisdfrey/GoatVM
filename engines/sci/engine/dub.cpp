/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "common/file.h"

#include "audio/audiostream.h"
#include "audio/mixer.h"
#include "audio/decoders/vorbis.h"

#include "sci/engine/dub.h"

namespace Sci {

SciDubManager::SciDubManager() : _lastTextOffset(0), _lastTextIndex(0) {}

void SciDubManager::setLastText(uint16 offset, uint16 index) {
	_lastTextOffset = offset;
	_lastTextIndex = index;
}

void SciDubManager::start() {
	Common::Path path = Common::Path(Common::String::format("dub/%d/%d.ogg", _lastTextOffset, _lastTextIndex));

	Common::File *dubFile = new Common::File();
	if (dubFile->exists(path)) {
		dubFile->open(path);

		Audio::RewindableAudioStream *audioStream = Audio::makeVorbisStream(dubFile, DisposeAfterUse::YES);
		g_system->getMixer()->playStream(Audio::Mixer::kSpeechSoundType, &_audioHandle, audioStream);
	}
}

void SciDubManager::stop() {
	g_system->getMixer()->stopHandle(_audioHandle);
}

} // End of namespace Sci
