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

SciDubManager::SciDubManager() : _curDubFile(nullptr) {}

void SciDubManager::loadConfig() {
	Common::File configFile;
	if (!configFile.open(Common::Path("dub/files.csv")))
		return;

	configFile.readLine(); // consume header line

	uint curFilesArrayIndex = 0;
	while (!configFile.eos() && !configFile.err()) {
		Common::String curLine = configFile.readLine();
		Common::Array<Common::String> columns = Common::StringTokenizer(curLine, ",").split();
		if (columns.size() < 5)
			continue;

		DubFileInfo info;
		info.type = columns[0];
		info.offset = (uint16)columns[1].asUint64();
		info.index = (uint16)columns[2].asUint64();
		info.hash = (uint16)columns[3].asUint64();
		info.stopOnClose = columns[4] != "0";
		info.path = columns[5];

		_dubFiles.push_back(info);

		if (info.type == "offset") {
			uint32 key = makeOffsetKey(info.offset, info.index);
			_offsetMap[key] = curFilesArrayIndex;
		} else if (info.type == "hash") {
			_hashMap[info.hash] = curFilesArrayIndex;
		}

		curFilesArrayIndex += 1;
	}
}

void SciDubManager::onTextLoad(const Common::String &text, uint16 offset, uint16 index) {
	if (text.size() == 0) {
		return;
	}

	if (offset != 0) {
		uint32 key = makeOffsetKey(offset, index);
		if (_offsetMap.contains(key)) {
			_playQueue.push(_offsetMap[key]);
		}
	} else {
		uint16 key = text.hash();
		if (_hashMap.contains(key)) {
			_playQueue.push(_hashMap[key]);
		}
	}
}

void SciDubManager::onTextOpen() {
	if (_playQueue.empty())
		return;

	uint filesArrayIndex = _playQueue.pop();
	_curDubFile = &(_dubFiles[filesArrayIndex]);

	Common::Path path = Common::Path(_curDubFile->path);

	Common::File *dubFile = new Common::File();
	if (dubFile->open(path)) {
		Audio::RewindableAudioStream *audioStream = Audio::makeVorbisStream(dubFile, DisposeAfterUse::YES);
		g_system->getMixer()->playStream(Audio::Mixer::kSpeechSoundType, &_audioHandle, audioStream);
	} else {
		delete dubFile;
	}
}

void SciDubManager::onTextClose() {
	if (_curDubFile && _curDubFile->stopOnClose) {
		g_system->getMixer()->stopHandle(_audioHandle);
	}
}

} // End of namespace Sci
