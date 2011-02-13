/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file aurora/lytfile.cpp
 *  Handling BioWare's LYTs (Layout files).
 */

#include "common/stream.h"
#include "common/streamtokenizer.h"
#include "common/util.h"

#include "aurora/lytfile.h"
#include "aurora/error.h"

namespace Aurora {

LYTFile::LYTFile() {
}

LYTFile::~LYTFile() {
}

void LYTFile::clear() {
	_rooms.clear();
	_doorHooks.clear();
	_fileDependency.clear();
}

void LYTFile::load(Common::SeekableReadStream &lyt) {
	clear();

	Common::StreamTokenizer tokenizer(Common::StreamTokenizer::kRuleIgnoreAll);
	tokenizer.addSeparator(' ');
	tokenizer.addChunkEnd('\n');
	tokenizer.addIgnore('\r');

	while (!lyt.eos() && !lyt.err()) {
		std::vector<Common::UString> strings;
		tokenizer.getTokens(lyt, strings);

		if (strings.empty()) {
			// Empty line?
		} else if (*strings[0].begin() == '#') {
			// Comment line
		} else if (strings[0] == "filedependancy") {
			// A clone2727 note: It's spelled "dependency", BioWare.
			_fileDependency = strings[1];
		} else if (strings[0] == "roomcount") {
			int roomCount = atoi(strings[1].c_str());
			_rooms.resize(roomCount);

			for (int i = 0; i < roomCount; i++) {
				tokenizer.nextChunk(lyt);
				tokenizer.getTokens(lyt, strings);

				_rooms[i].model = strings[0];
				strings[1].parse(_rooms[i].x);
				strings[2].parse(_rooms[i].y);
				strings[3].parse(_rooms[i].z);
			}
		} else if (strings[0] == "trackcount") {
			int trackCount = atoi(strings[1].c_str());

			// TODO! Just ignore for now
			for (int i = 0; i < trackCount; i++) {
				tokenizer.nextChunk(lyt);
				//tokenizer.getTokens(lyt, strings);
			}
		} else if (strings[0] == "obstaclecount") {
			int obstacleCount = atoi(strings[1].c_str());

			// TODO! Just ignore for now
			for (int i = 0; i < obstacleCount; i++) {
				tokenizer.nextChunk(lyt);
				//tokenizer.getTokens(lyt, strings);
			}
		} else if (strings[0] == "artplaceablecount") {
			int obstacleCount = atoi(strings[1].c_str());

			// TODO! Just ignore for now
			for (int i = 0; i < obstacleCount; i++) {
				tokenizer.nextChunk(lyt);
				//tokenizer.getTokens(lyt, strings);
			}
		} else if (strings[0] == "walkmeshRooms") {
			int obstacleCount = atoi(strings[1].c_str());

			// TODO! Just ignore for now
			for (int i = 0; i < obstacleCount; i++) {
				tokenizer.nextChunk(lyt);
				//tokenizer.getTokens(lyt, strings);
			}
		} else if (strings[0] == "doorhookcount") {
			int doorHookCount = atoi(strings[1].c_str());
			_doorHooks.resize(doorHookCount);

			for (int i = 0; i < doorHookCount; i++) {
				tokenizer.nextChunk(lyt);
				tokenizer.getTokens(lyt, strings);

				_doorHooks[i].name = strings[0];
				strings[1].parse(_doorHooks[i].unk0);
				strings[2].parse(_doorHooks[i].x);
				strings[3].parse(_doorHooks[i].y);
				strings[4].parse(_doorHooks[i].z);
				strings[5].parse(_doorHooks[i].unk1);
				strings[6].parse(_doorHooks[i].unk2);
				strings[7].parse(_doorHooks[i].unk3);
				strings[8].parse(_doorHooks[i].unk4);
				strings[9].parse(_doorHooks[i].unk5);
			}
		} else if (strings[0] == "beginlayout") {
			// Ignore, we don't need it
		} else if (strings[0] == "donelayout") {
			// End parsing
			break;
		} else {
			throw Common::Exception("Unknown LYT token %s", strings[0].c_str());
		}

		tokenizer.nextChunk(lyt);
	}
}

const LYTFile::RoomArray &LYTFile::getRooms() const {
	return _rooms;
}

const LYTFile::DoorHookArray &LYTFile::getDoorHooks() const {
	return _doorHooks;
}

Common::UString LYTFile::getFileDependency() const {
	return _fileDependency;
}

} // End of namespace Aurora
