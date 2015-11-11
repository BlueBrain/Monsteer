/* Copyright (c) 2011-2015, EPFL/Blue Brain Project
 *                          Jafet Villafranca <jafet.villafrancadiaz@epfl.ch>
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

#include <monsteer/qt/SteeringWidget.h>
#include <monsteer/qt/GeneratorModel.h>
#include <monsteer/qt/GeneratorPropertiesModel.h>
#include <monsteer/qt/PropertyEditDelegate.h>
#include <monsteer/qt/nestData.h>

#include <monsteer/steering/simulator.h>
#include <monsteer/ui_SteeringWidget.h>

#include <zeq/subscriber.h>
#include <zeq/hbp/vocabulary.h>

#include <QTimer>

#include <boost/bind.hpp>
#include <boost/foreach.hpp>

namespace monsteer
{
namespace qt
{

struct SteeringWidget::Impl
{
    Impl( SteeringWidget* steeringWidget,
          const servus::URI& selectionZeqSchema,
          const servus::URI& simulationZeqSchema )
        : _simulator( 0 )
        , _selectionSubscriber( new zeq::Subscriber( selectionZeqSchema ))
        , _playing( true )
        , _isRegistered( false )
        , _steeringWidget( steeringWidget )
        , _simulationZeqSchema( simulationZeqSchema )
    {
        _ui.setupUi( steeringWidget );
        _ui.tblGeneratorProperties->horizontalHeader()->setSectionResizeMode( QHeaderView::Stretch );

        connectHBP();
        connectISC();

        _receiveTimer.connect( &_receiveTimer, &QTimer::timeout,
                               [this](){ _selectionSubscriber->receive( 0 ); });
        _receiveTimer.start( 100 );
    }

    ~Impl()
    {
        _receiveTimer.stop();
        disconnectISC();
        disconnectHBP();
    }

    void injectStimuli()
    {
        if( !_simulator )
            return;

        if( _selectedIds.empty( ))
            return;

        GeneratorPropertiesModel *model =
                static_cast< GeneratorPropertiesModel * >(
                    _ui.tblGeneratorProperties->model( ));

        const int generatorIndex = _ui.generatorsComboBox->currentIndex();
        if( generatorIndex < 0 )
            return;

        const Strings& generators = getGenerators();
        const std::string& generator = generators[ generatorIndex ];

        PropertyList properties = model->getProperties( );
        QVariantList _list;
        _list.push_back( QVariant(QString(generator.c_str( ))));
        properties.push_back( std::make_pair("model", _list ));

        const std::string& json = getJSON( properties );
        if( json.empty( ))
            return;

        _simulator->injectStimulus( json, _selectedIds );
    }

    void playPauseSimulation()
    {
        if( !_simulator )
            return;

        if( _playing )
        {
            _simulator->pause();
            _ui.btnPlayPauseSimulation->setText( "Resume simulation" );
        }
        else
        {
            _simulator->play();
            _ui.btnPlayPauseSimulation->setText( "Pause simulation" );
        }
        _playing = !_playing;
    }

    void generatorSelected( const int generatorIndex )
    {
        if( generatorIndex < 0 )
            return;

        const Strings& generators = getGenerators();
        const PropertyList& prop = getGeneratorProperties( generators[ generatorIndex ] );
        GeneratorPropertiesModel* model =
                static_cast< GeneratorPropertiesModel* >( _ui.tblGeneratorProperties->model( ));

        model->setProperties( prop );
    }

    void connectISC()
    {
        try
        {
            _simulator = new monsteer::Simulator( _simulationZeqSchema );
            _ui.btnPlayPauseSimulation->setEnabled( true );
        }
        catch( const std::exception& error )
        {
            _ui.btnInjectStimulus->setEnabled( false );
            _ui.btnPlayPauseSimulation->setEnabled( false );
            LBERROR << "Error:" << error.what() << std::endl;
        }
    }

    void disconnectISC()
    {
        delete _simulator;
    }

    void onSelection( const ::zeq::Event& event_ )
    {
        const std::vector<uint32_t>& selection
                = zeq::hbp::deserializeSelectedIDs( event_ );
        emit _steeringWidget->updateCellIdsTextBox( selection );
    }

    void updateCellIdsTextBox( const std::vector<uint32_t>& cellIds )
    {
        _selectedIds = cellIds;

        const int generatorIndex = _ui.generatorsComboBox->currentIndex();

        _ui.btnInjectStimulus->setEnabled( _simulator &&
                                           !_selectedIds.empty( ) &&
                                           generatorIndex >= 0 );

        std::stringstream str;
        BOOST_FOREACH( const uint32_t gid, _selectedIds )
                str << gid << " ,";

        _ui.txtCellIds->setText( str.str().c_str( ));
    }

    void connectHBP()
    {
        try
        {
            _selectionSubscriber->registerHandler(
                        zeq::hbp::EVENT_SELECTEDIDS,
                        boost::bind( &SteeringWidget::Impl::onSelection,
                                     this, _1 ));
            _isRegistered = true;
        }
        catch( const std::exception& error )
        {
            LBERROR << "Error:" << error.what() << std::endl;
            _isRegistered = false;
        }
    }

    void disconnectHBP()
    {
        _selectionSubscriber->deregisterHandler( zeq::hbp::EVENT_SELECTEDIDS );
        _isRegistered = false;
    }

    void propertiesChanged()
    {
        GeneratorPropertiesModel* model =
                static_cast< GeneratorPropertiesModel* >(_ui.tblGeneratorProperties->model());

        for( int i = 0; i < model->rowCount(); ++i )
        {
            _ui.tblGeneratorProperties->closePersistentEditor( model->index( i, 1 ));
            _ui.tblGeneratorProperties->openPersistentEditor( model->index( i, 1 ));
        }
    }

public:
    ::monsteer::Simulator* _simulator;
    zeq::Subscriber* _selectionSubscriber;
    bool _playing;
    bool _isRegistered;
    Ui_steeringWidget _ui;
    SteeringWidget* _steeringWidget;
    servus::URI _simulationZeqSchema;

    QTimer _receiveTimer;
    std::vector<uint32_t> _selectedIds;
};

SteeringWidget::SteeringWidget( const servus::URI& selectionZeqSchema,
                                const servus::URI& simulationZeqSchema,
                                QWidget* parentWgt )
    : QWidget( parentWgt )
    , _impl( new SteeringWidget::Impl( this, selectionZeqSchema,
                                       simulationZeqSchema ))
{
    qRegisterMetaType< std::vector<uint32_t> >("std::vector<uint32_t>");

    _impl->_ui.txtCellIds->setReadOnly( true );
    GeneratorModel* generatorModel =
            new GeneratorModel( _impl->_ui.generatorsComboBox );
    _impl->_ui.generatorsComboBox->setModel( generatorModel );

    PropertyEditDelegate* editorDelegate =
            new PropertyEditDelegate( _impl->_ui.tblGeneratorProperties );
    _impl->_ui.tblGeneratorProperties->setItemDelegate( editorDelegate );

    GeneratorPropertiesModel *model =
            new GeneratorPropertiesModel( _impl->_ui.tblGeneratorProperties );

    _impl->_ui.tblGeneratorProperties->setModel( model );
    _impl->_ui.tblGeneratorProperties->setColumnWidth( 0, 150 );
    _impl->_ui.tblGeneratorProperties->setColumnWidth( 1, 150 );
    _impl->_ui.tblGeneratorProperties->setMinimumWidth( 310 );

    _impl->_ui.btnInjectStimulus->setEnabled( false );

    setWindowFlags(( windowFlags() ^ Qt::Dialog) | Qt::Window );

    connect( _impl->_ui.btnPlayPauseSimulation, SIGNAL(clicked()),
             this, SLOT(_playPauseSimulation()));

    connect( _impl->_ui.btnInjectStimulus, SIGNAL(clicked()),
             this, SLOT(_injectStimuli()));

    connect( _impl->_ui.generatorsComboBox, SIGNAL( currentIndexChanged( int )),
             this, SLOT( _generatorSelected( int )));

    connect( model, SIGNAL( layoutChanged( )),
             this, SLOT( _propertiesChanged( )));

    connect( this, SIGNAL( updateCellIdsTextBox( std::vector<uint32_t> )),
             this, SLOT( _updateCellIdsTextBox( std::vector<uint32_t> )),
             Qt::QueuedConnection );

    _impl->_ui.generatorsComboBox->setCurrentIndex( 0 );
    // Emulate currentIndexChanged() signal as qApp has not been created yet
    _impl->generatorSelected( 0 );
}

SteeringWidget::~SteeringWidget( )
{
    delete _impl;
}

void SteeringWidget::_injectStimuli()
{
    _impl->injectStimuli();
}

void SteeringWidget::_playPauseSimulation()
{
    _impl->playPauseSimulation();
}

void SteeringWidget::_generatorSelected( const int index )
{
    _impl->generatorSelected( index );
}

void SteeringWidget::_updateCellIdsTextBox( std::vector<uint32_t> cellIds )
{
    _impl->updateCellIdsTextBox( cellIds );
}

void SteeringWidget::_propertiesChanged()
{
    _impl->propertiesChanged();
}

}
}
