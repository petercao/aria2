/*
 * aria2 - The high speed download utility
 *
 * Copyright (C) 2006 Tatsuhiro Tsujikawa
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
 */
/* copyright --> */
#include "DelegatingPeerListProcessor.h"
#include "DefaultPeerListProcessor.h"
#include "CompactPeerListProcessor.h"
#include "Peer.h"
#include <algorithm>

namespace aria2 {

DelegatingPeerListProcessor::DelegatingPeerListProcessor(int32_t pieceLength, int64_t totalLength):
  pieceLength(pieceLength),
  totalLength(totalLength)
{
  processors.push_back(new DefaultPeerListProcessor(pieceLength, totalLength));
  processors.push_back(new CompactPeerListProcessor(pieceLength, totalLength));
}

DelegatingPeerListProcessor::~DelegatingPeerListProcessor() {}

Peers DelegatingPeerListProcessor::extractPeer(const MetaEntry* peersEntry) {
  Peers peers;
  for(std::deque<SharedHandle<PeerListProcessor> >::iterator itr = processors.begin();
      itr != processors.end(); itr++) {
    PeerListProcessorHandle processor = *itr;
    if(processor->canHandle(peersEntry)) {
      Peers tempPeers = processor->extractPeer(peersEntry);
      std::copy(tempPeers.begin(), tempPeers.end(), std::back_inserter(peers));
      break;
    }
  }
  return peers;
}

bool DelegatingPeerListProcessor::canHandle(const MetaEntry* peersEntry) const {
  for(std::deque<SharedHandle<PeerListProcessor> >::const_iterator itr = processors.begin();
      itr != processors.end(); itr++) {
    if((*itr)->canHandle(peersEntry)) {
      return true;
    }
  }
  return false;
}

} // namespace aria2
