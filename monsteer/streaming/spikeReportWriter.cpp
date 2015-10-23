
/* Copyright (c) 2006-2015, Juan Hernando <jhernando@fi.upm.es>
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

#include "spikeReportWriter.h"
#include "spikes.h"

#include <brion/spikeReport.h>

namespace monsteer
{

class SpikeReportWriter::_Impl
{
public:
    _Impl( const brion::URI& uri, const int accessMode )
        : _report( uri, accessMode )
    {}

    brion::SpikeReport _report;
};

SpikeReportWriter::SpikeReportWriter( const brion::URI& uri,
                                      const int accessMode )
    : _impl( new _Impl( uri, accessMode ))
{
}

SpikeReportWriter::~SpikeReportWriter()
{
    delete _impl;
}

void SpikeReportWriter::writeSpikes( const Spikes &spikes )
{
    brion::Spikes brionSpikes;
    brionSpikes.insert( spikes.begin(), spikes.end() );
    _impl->_report.writeSpikes( brionSpikes );
}

const lunchbox::URI& SpikeReportWriter::getURI() const
{
    return _impl->_report.getURI();
}

void SpikeReportWriter::close()
{
    _impl->_report.close();
}

}

