/**
 * Copyright (c) 2018 Inria
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Daniel Carvalho
 */

#include "mem/cache/replacement_policies/l1_miss_rp.hh"

#include <cassert>
#include <memory>

#include "params/L1MISSRP.hh"

L1MISSRP::L1MISSRP(const Params *p)
    : BaseReplacementPolicy(p)
{L1MISSRP::isL1MISSRP=true;
}

void
L1MISSRP::invalidate(const std::shared_ptr<ReplacementData>& replacement_data)
const
{
    // Reset last touch timestamp
    std::static_pointer_cast<L1MISSReplData>(
        replacement_data)->l1MissTick = Tick(0);
    std::static_pointer_cast<L1MISSReplData>(
        replacement_data)->inL1 = 0;
}

void
L1MISSRP::touch(const std::shared_ptr<ReplacementData>& replacement_data) const
{
    // Update last touch timestamp
    std::static_pointer_cast<L1MISSReplData>(
        replacement_data)->l1MissTick = curTick();
    std::static_pointer_cast<L1MISSReplData>(
        replacement_data)->inL1 = 0;

}

void
L1MISSRP::reset(const std::shared_ptr<ReplacementData>& replacement_data) const
{
    // Set last touch timestamp
    // TODO: think about the tick during insert
    std::static_pointer_cast<L1MISSReplData>(
        replacement_data)->l1MissTick = curTick();
    std::static_pointer_cast<L1MISSReplData>(
        replacement_data)->inL1 = 1;
}

ReplaceableEntry*
L1MISSRP::getVictim(const ReplacementCandidates& candidates) const
{
    // There must be at least one replacement candidate
    assert(candidates.size() > 0);

    // Visit all candidates to find victim
    ReplaceableEntry* victim = candidates[0];

    for (const auto& candidate : candidates) {
        if (std::static_pointer_cast<L1MISSReplData>(
                    candidate->replacementData)->inL1==0) {
                victim=candidate;
                break;
        }
    }
    for (const auto& candidate : candidates) {
        if (std::static_pointer_cast<L1MISSReplData>(
                    candidate->replacementData)->inL1==0 &&
(std::static_pointer_cast<L1MISSReplData>(
                    candidate->replacementData)->l1MissTick <
                std::static_pointer_cast<L1MISSReplData>(
                    victim->replacementData)->l1MissTick)) {
            victim = candidate;
        }
    }

    return victim;
}

std::shared_ptr<ReplacementData>
L1MISSRP::instantiateEntry()
{
    return std::shared_ptr<ReplacementData>(new L1MISSReplData());
}

L1MISSRP*
L1MISSRPParams::create()
{
    return new L1MISSRP(this);
}
