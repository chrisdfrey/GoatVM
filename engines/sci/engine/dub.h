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

#ifndef SCI_ENGINE_DUB_H
#define SCI_ENGINE_DUB_H

#include "common/hashmap.h"
#include "common/queue.h"
#include "common/str.h"

#include "audio/mixer.h"

namespace Sci {

class SciDubManager {
public:
	SciDubManager();

	void setMessage(const Common::String &text, uint16 offset=0, uint16 index=0);

	void loadConfig();
	void start();
	void stop();

private:
	Common::HashMap<uint32, Common::String> _offsetMap;
	Common::HashMap<uint16, Common::String> _hashMap;

	Common::Queue<Common::String> _dubFileQueue;

	Audio::SoundHandle _audioHandle;
};

} // End of namespace Sci

#endif
