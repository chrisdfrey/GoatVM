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

#include "common/array.h"
#include "common/file.h"
#include "common/tokenizer.h"

#include "audio/audiostream.h"
#include "audio/mixer.h"
#include "audio/decoders/vorbis.h"

#include "sci/engine/dub.h"

namespace Sci {

uint32 makeOffsetKey(uint16 offset, uint16 index) {
	return ((uint32)offset << 16) + index;
}

SciDubManager::SciDubManager() {}

void SciDubManager::setMessage(const Common::String &text, uint16 offset, uint16 index) {
	if (text.size() == 0) {
		return;
	}

	if (offset != 0) {
		uint32 key = makeOffsetKey(offset, index);
		if (_offsetMap.contains(key)) {
			_dubFileQueue.push(_offsetMap[key]);
		}
	} else {
		uint16 key = text.hash();
		if (_hashMap.contains(key)) {
			_dubFileQueue.push(_hashMap[key]);
		}
	}
}

void SciDubManager::loadConfig() {
	Common::File configFile;
	if (!configFile.open(Common::Path("dub/files.csv")))
		return;

	while (!configFile.eos() && !configFile.err()) {
		Common::Array<Common::String> columns = Common::StringTokenizer(configFile.readLine(), ",").split();
		if (columns.size() < 5)
			continue;

		Common::String type = columns[0];
		Common::String path = columns[4];

		if (type == "offset") {
			uint16 offset = columns[1].asUint64();
			uint16 index = columns[2].asUint64();
		
			uint32 key = makeOffsetKey(offset, index);
			_offsetMap[key] = path;
		} else if (type == "hash") {
			uint16 key = (uint16)columns[3].asUint64();

			_hashMap[key] = path;
		}
	}
}

void SciDubManager::start() {
	if (_dubFileQueue.empty())
		return;

	Common::Path path(_dubFileQueue.pop());

	Common::File *dubFile = new Common::File();
	if (dubFile->exists(path) && dubFile->open(path)) {
		Audio::RewindableAudioStream *audioStream = Audio::makeVorbisStream(dubFile, DisposeAfterUse::YES);
		g_system->getMixer()->playStream(Audio::Mixer::kSpeechSoundType, &_audioHandle, audioStream);
	} else {
		delete dubFile;
	}
}

void SciDubManager::stop() {
	g_system->getMixer()->stopHandle(_audioHandle);
}

} // End of namespace Sci
