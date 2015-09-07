
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

#ifndef MONSTEER_SPIKEREPORTWRITER_H
#define MONSTEER_SPIKEREPORTWRITER_H

#include <monsteer/types.h>

#include <boost/noncopyable.hpp>

namespace monsteer
{

/**
 * Writer for spike data.
 *
 * Following RAII, a writer is ready for use after the creation and will
 * ensure release of resources upon destruction.
 *
 * @version 0.2
 */
class SpikeReportWriter : public boost::noncopyable
{
public:
    /**
     * Construct a new writer for the given uri.
     * @param uri URI to spike report
     * @param accessMode Access mode
     * @version 0.2
     */
    SpikeReportWriter( const brion::URI& uri,
                       const int accessMode = brion::MODE_WRITE );

    /**
     * Destructor.
     * @version 0.2
     */
    ~SpikeReportWriter();

    /**
     * Writes the spike times and cell GIDs.
     *
     * @param spikes Spikes to write.
     * @version 0.2 */
    void writeSpikes( const Spikes& spikes );

    /**
     * Closes the report. ( It is implicitly called on destruction ).
     *
     * @version 0.2 */
    void close();

private:
    class _Impl;
    _Impl* _impl;
};

}
#endif
