#pragma once

#include <actions/WidgetAction.h>
#include <actions/GroupsAction.h>
#include <actions/TriggerAction.h>
#include <actions/IntegralAction.h>

#include <QString>
#include <QStringList>
#include <QLabel>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QComboBox>

using namespace hdps;
using namespace hdps::gui;

class PCPSettings;

class DimensionSelectionAction : public WidgetAction
{
protected:

    class Widget : public hdps::gui::WidgetActionWidget {
    public:
        Widget(QWidget* parent, DimensionSelectionAction* layersAction);

    protected:
        GroupsAction    _groupsAction;
    };

    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return new Widget(parent, this);
    };

public:
    DimensionSelectionAction(PCPSettings& PCPSettings);

    void setDimensionNames(QStringList dimNames);
    QStringList getSelectedDimensionNames();
    std::vector<bool> getSelectedDimensions();

    void setNumPoints(int32_t num);
    void setNumDims(int32_t num);
    void setNumItems(int32_t num);

public: // Action getters

    QComboBox* getDimensionSelection() { return &_dimensionSelection; }
    TriggerAction& getLoadAction() { return _loadAction; }
    TriggerAction& getApplyAction() { return _applyAction; }
    IntegralAction& getNumPointsAction() { return _numPointsAction; }
    IntegralAction& getNumDimsAction() { return _numDimsAction; }
    IntegralAction& getNumItemsAction() { return _numItemsAction; }

private slots:
    void onLoadDims();

private:
    bool disableAllDimensions();
    bool tryToEnableDimensionByName(QString name);

protected:
    PCPSettings& _parallelCoordinatesSettings;

    // For ComboBoxOfCheckBoxes, see http://programmingexamples.net/wiki/Qt/ModelView/ComboBoxOfCheckBoxes
    QStandardItemModel          _comboBoxLineModel;
    //std::vector<QStandardItem*> _comboBoxItems;
    QStringList                 _dimNames;

    QComboBox                   _dimensionSelection;
    TriggerAction               _loadAction;
    TriggerAction               _applyAction;
    IntegralAction              _numPointsAction;
    IntegralAction              _numDimsAction;
    IntegralAction              _numItemsAction;

};