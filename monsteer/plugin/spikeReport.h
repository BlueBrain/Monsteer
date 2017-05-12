
/* Copyright (c) 2006-2017, Juan Hernando <jhernando@fi.upm.es>
 *                          Ahmet Bilgili <ahmet.bilgili@epfl.ch>
 *
 * This file is part of Monsteer <https://github.com/BlueBrain/Monsteer>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef MONSTEER_PLUGIN_SPIKEREPORT_H
#define MONSTEER_PLUGIN_SPIKEREPORT_H

#include <monsteer/plugin/endOfStream.h>
#include <monsteer/plugin/spikes.h>
#include <monsteer/types.h>

#include <zeroeq/types.h>

#include <brion/spikeReportPlugin.h>

namespace monsteer
{
namespace plugin
{
using brion::SpikeReportInitData;

/** A ZeroEQ streaming spike report reader/writer. Class is not thread safe. */
class SpikeReport : public brion::SpikeReportPlugin
{
public:
    /** Create a new streaming NEST report. */
    explicit SpikeReport(const SpikeReportInitData &initData);

    /** Check if this plugin can handle the given plugin data. */
    static bool handles(const SpikeReportInitData &initData);
    static std::string getDescription();

    void close() final;
    brion::Spikes read(float min) final;
    brion::Spikes readUntil(float max) final;
    void readSeek(float toTimeStamp) final;
    void writeSeek(float toTimeStamp) final;
    void write(const brion::Spike *spikes, const size_t size) final;
    bool supportsBackwardSeek() const final { return false; }
private:
    void _onSpikes(ConstSpikesEventPtr event);
    void _onSeekForward(ConstSeekForwardEventPtr event);
    void _onEOS();
    void _receiveBufferedMessages();

    brion::Spikes _spikes;
    std::unique_ptr<zeroeq::Subscriber> _subscriber;
    std::unique_ptr<zeroeq::Publisher> _publisher;
    float _publisherTimeStamp = -std::numeric_limits<float>::infinity();
    bool _publisherFinished = false;
};
}
} // namespaces
#endif
