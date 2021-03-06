/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names
 * can be found in the AUTHORS file distributed with this source
 * distribution.
 *
 * xoreos is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * xoreos is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with xoreos. If not, see <http://www.gnu.org/licenses/>.
 */

// Largely based on the AudioStream implementation found in ScummVM.

/** @file
 *  Streaming audio.
 */

#ifndef SOUND_AUDIOSTREAM_H
#define SOUND_AUDIOSTREAM_H

#include "src/common/util.h"
#include "src/common/types.h"

namespace Sound {

/**
 * Generic audio input stream. Subclasses of this are used to feed arbitrary
 * sampled audio data into xoreos' SoundManager.
 */
class AudioStream {
public:
	static const size_t kSizeInvalid = SIZE_MAX;

	virtual ~AudioStream() {}

	/**
	 * Fill the given buffer with up to numSamples samples. Returns the actual
	 * number of samples read, or kSizeInvalid if a critical error occurred
	 * (note: you *must* check if this value is less than what you requested,
	 * this can happen when the stream is fully used up).
	 *
	 * Data has to be in native endianness, 16 bit per sample, signed. For stereo
	 * stream, buffer will be filled with interleaved left and right channel
	 * samples, starting with a left sample. Furthermore, the samples in the
	 * left and right are summed up. So if you request 4 samples from a stereo
	 * stream, you will get a total of two left channel and two right channel
	 * samples.
	 *
	 * The same holds true for more channels. Channel configurations recognized:
	 * - 5.1: front left, front right, front center, low frequency rear left, rear right
	 */
	virtual size_t readBuffer(int16 *buffer, size_t numSamples) = 0;

	/** Return the number channels in this stream. */
	virtual int getChannels() const = 0;

	/** Sample rate of the stream. */
	virtual int getRate() const = 0;

	/**
	 * End of data reached? If this returns true, it means that at this
	 * time there is no data available in the stream. However there may be
	 * more data in the future.
	 * This is used by e.g. a rate converter to decide whether to keep on
	 * converting data or stop.
	 */
	virtual bool endOfData() const = 0;

	/**
	 * End of stream reached? If this returns true, it means that all data
	 * in this stream is used up and no additional data will appear in it
	 * in the future.
	 * This is used by the mixer to decide whether a given stream shall be
	 * removed from the list of active streams (and thus be destroyed).
	 * By default this maps to endOfData()
	 */
	virtual bool endOfStream() const { return endOfData(); }
};

/**
 * A rewindable audio stream. This allows for reseting the AudioStream
 * to its initial state. Note that rewinding itself is not required to
 * be working when the stream is being played by Mixer!
 */
class RewindableAudioStream : public AudioStream {
public:
	/**
	 * Rewinds the stream to its start.
	 *
	 * @return true on success, false otherwise.
	 */
	virtual bool rewind() = 0;
};

/**
 * A looping audio stream. This object does nothing besides using
 * a RewindableAudioStream to play a stream in a loop.
 */
class LoopingAudioStream : public AudioStream {
public:
	/**
	 * Creates a looping audio stream object.
	 *
	 * @see makeLoopingAudioStream
	 *
	 * @param stream Stream to loop
	 * @param loops How often to loop (0 = infinite)
	 * @param disposeAfterUse Destroy the stream after the LoopingAudioStream has finished playback.
	 */
	LoopingAudioStream(RewindableAudioStream *stream, size_t loops, bool disposeAfterUse = true);
	~LoopingAudioStream();

	size_t readBuffer(int16 *buffer, const size_t numSamples);
	bool endOfData() const;

	int getChannels() const { return _parent->getChannels(); }
	int getRate() const { return _parent->getRate(); }

	/** Returns number of loops the stream has played. */
	size_t getCompleteIterations() const { return _completeIterations; }
private:
	RewindableAudioStream *_parent;
	bool _disposeAfterUse;

	size_t _loops;
	size_t _completeIterations;
};

/**
 * Wrapper functionality to efficiently create a stream, which might be looped.
 *
 * Note that this function does not return a LoopingAudioStream, because it does
 * not create one when the loop count is "1". This allows to keep the runtime
 * overhead down, when the code does not require any functionality only offered
 * by LoopingAudioStream.
 *
 * @param stream Stream to loop (will be automatically destroyed, when the looping is done)
 * @param loops How often to loop (0 = infinite)
 * @return A new AudioStream, which offers the desired functionality.
 */
AudioStream *makeLoopingAudioStream(RewindableAudioStream *stream, size_t loops);

class QueuingAudioStream : public AudioStream {
public:

	/**
	 * Queue an audio stream for playback. This stream plays all queued
	 * streams, in the order they were queued. If disposeAfterUse is set to
	 * DisposeAfterUse::YES, then the queued stream is deleted after all data
	 * contained in it has been played.
	 */
	virtual void queueAudioStream(AudioStream *audStream, bool disposeAfterUse = true) = 0;

	/**
	 * Mark this stream as finished. That is, signal that no further data
	 * will be queued to it. Only after this has been done can this
	 * stream ever 'end'.
	 */
	virtual void finish() = 0;

	/**
	 * Return the number of streams still queued for playback (including
	 * the currently playing stream).
	 */
	virtual size_t numQueuedStreams() const = 0;
};

/**
 * Factory function for an QueuingAudioStream.
 */
QueuingAudioStream *makeQueuingAudioStream(int rate, int channels);

} // End of namespace Sound

#endif // SOUND_AUDIOSTREAM_H
