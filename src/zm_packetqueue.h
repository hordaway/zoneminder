//ZoneMinder Packet Queue Interface Class
//Copyright 2016 Steve Gilvarry
//
//This file is part of ZoneMinder.
//
//ZoneMinder is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//ZoneMinder is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with ZoneMinder.  If not, see <http://www.gnu.org/licenses/>.


#ifndef ZM_PACKETQUEUE_H
#define ZM_PACKETQUEUE_H

//#include <boost/interprocess/managed_shared_memory.hpp>
//#include <boost/interprocess/containers/map.hpp>
//#include <boost/interprocess/allocators/allocator.hpp>
#include <list>
#include "zm_packet.h"
#include "zm_thread.h"
#include <mutex>
#include <condition_variable>

extern "C" {
#include <libavformat/avformat.h>
}
typedef std::list<ZMPacket *>::iterator packetqueue_iterator;

class zm_packetqueue {
  public: // For now just to ease development
    std::list<ZMPacket *>    pktQueue;
    std::list<ZMPacket *>::iterator analysis_it;

    int video_stream_id;
    int max_video_packet_count; // allow a negative value to someday mean unlimited
    int max_stream_id;
    int *packet_counts;     /* packet count for each stream_id, to keep track of how many video vs audio packets are in the queue */
    bool deleting;
    std::list<packetqueue_iterator *> iterators;

    std::mutex mutex;
    std::condition_variable condition;

  public:
    zm_packetqueue(int p_max_video_packet_count, int p_video_stream_id, int p_audio_stream_id);
    virtual ~zm_packetqueue();
    std::list<ZMPacket *>::const_iterator end() const { return pktQueue.end(); }
    std::list<ZMPacket *>::const_iterator begin() const { return pktQueue.begin(); }

    bool queuePacket(ZMPacket* packet);
    ZMPacket * popPacket();
    bool popVideoPacket(ZMPacket* packet);
    bool popAudioPacket(ZMPacket* packet);
    unsigned int clearQueue(unsigned int video_frames_to_keep, int stream_id);
    unsigned int clearQueue(struct timeval *duration, int streamid);
    void clearQueue();
    void dumpQueue();
    unsigned int size();
    unsigned int get_packet_count(int stream_id) const { return packet_counts[stream_id]; };

    void clear_unwanted_packets(timeval *recording, int pre_event_count, int mVideoStreamId);
    int packet_count(int stream_id);

    bool increment_it(packetqueue_iterator *it);
    bool increment_it(packetqueue_iterator *it, int stream_id);
    ZMPacket *get_packet(packetqueue_iterator *);
    packetqueue_iterator *get_video_it(bool wait);
    packetqueue_iterator *get_stream_it(int stream_id);

    std::list<ZMPacket *>::iterator get_event_start_packet_it(
        packetqueue_iterator snapshot_it,
        unsigned int pre_event_count
    );
};

#endif /* ZM_PACKETQUEUE_H */
