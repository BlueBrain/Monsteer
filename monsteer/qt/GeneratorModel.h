/* Copyright (c) 2011-2015, EPFL/Blue Brain Project
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

#ifndef MONSTEER_QT_GENERATORMODEL_H
#define MONSTEER_QT_GENERATORMODEL_H

#include <monsteer/qt/types.h>

#include <QAbstractTableModel>

namespace monsteer
{
namespace qt
{
/**
 * Keeps the data for generator table view.
 */
class GeneratorModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    /**
     * @param parentObj Parent widget.
     */
    explicit GeneratorModel(QObject* parentObj);
    ~GeneratorModel();

    int columnCount(const QModelIndex& parent = QModelIndex()) const final;
    int rowCount(const QModelIndex& parent = QModelIndex()) const final;
    QVariant data(const QModelIndex& index,
                  int role = Qt::DisplayRole) const final;
    Qt::ItemFlags flags(const QModelIndex& /*index*/) const final;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role) const final;
};
}
}
#endif
