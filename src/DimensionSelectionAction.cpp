#include "DimensionSelectionAction.h"
#include "ParallelCoordinatesSettings.h"
#include "ParallelCoordinatesPlugin.h"

#include <Application.h>

#include <QDebug>
#include <QFileDialog>


DimensionSelectionAction::DimensionSelectionAction(PCPSettings& parallelCoordinatesSettings) :
    WidgetAction(&parallelCoordinatesSettings),
    _parallelCoordinatesSettings(parallelCoordinatesSettings),
    _loadAction(this, "Load dimension selection"),
    _applyAction(this, "Apply"),
    _numPointsAction(this, "Number of points"),
    _numDimsAction(this, "Number of dimensions"),
    _numItemsAction(this, "Number of total items"),
    _comboBoxLineModel(),
    _dimensionSelection()
{
    setText("Dimension Selection");
    setIcon(Application::getIconFont("FontAwesome").getIcon("layer-group"));

    _numPointsAction.setDefaultWidgetFlags(IntegralAction::LineEdit);
    _numDimsAction.setDefaultWidgetFlags(IntegralAction::LineEdit);
    _numItemsAction.setDefaultWidgetFlags(IntegralAction::LineEdit);

    _numPointsAction.initialize(0, 7'000'000, 1, 1);
    _numDimsAction.initialize(0, 7'000'000, 1, 1);
    _numItemsAction.initialize(0, 7'000'000, 1, 1);

    _numPointsAction.setEnabled(false);
    _numDimsAction.setEnabled(false);
    _numItemsAction.setEnabled(false);

    _numItemsAction.setToolTip("#points * #dims must be smaller than 7,000,000");

    _dimensionSelection.setModel(&_comboBoxLineModel);
    _dimensionSelection.setMinimumWidth(100);
    _dimensionSelection.setSizeAdjustPolicy(QComboBox::AdjustToContents);

    // only show selected dimensions
    connect(&_loadAction, &TriggerAction::triggered, this, &DimensionSelectionAction::onLoadDims);
    connect(&_applyAction, &TriggerAction::triggered, &_parallelCoordinatesSettings, &PCPSettings::onApplyDimensionFiltering);
}

void DimensionSelectionAction::setDimensionNames(QStringList dimNames) {
    _dimNames = dimNames;
    auto numDims = _dimNames.length();

    _comboBoxLineModel.clear();

    // create new checkable items in the combo box
    for (size_t i = 0; i < numDims; i++) {
        // Reduce clutter by only showing numbers if there are many dimensions
        // Therefor, enumerate the dimensions for the user the easier reference them
        QStandardItem* item = new QStandardItem(QString("%0: %1").arg(i).arg(_dimNames[i]));

        item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        item->setData(Qt::Checked, Qt::CheckStateRole);

        _comboBoxLineModel.appendRow(item);
    }

}

QStringList DimensionSelectionAction::getSelectedDimensionNames() {
    QStringList selectedDimensionNames;

    for (size_t i = 0; i < _comboBoxLineModel.rowCount(); i++) {
        if (_comboBoxLineModel.item(i)->checkState())
        {
            selectedDimensionNames.push_back(_dimNames[i]);
        }
    }

    return selectedDimensionNames;
}

std::vector<bool> DimensionSelectionAction::getSelectedDimensions() {
    std::vector<bool> selectedDimensions;

    for (size_t i = 0; i < _comboBoxLineModel.rowCount(); i++) {
        selectedDimensions.push_back(_comboBoxLineModel.item(i)->checkState());
    }

    return selectedDimensions;
}

void DimensionSelectionAction::setNumPoints(int32_t num) {
    _numPointsAction.setValue(num);
}

void DimensionSelectionAction::setNumDims(int32_t num) {
    _numDimsAction.setValue(num);
}

void DimensionSelectionAction::setNumItems(int32_t num) {
    _numItemsAction.setValue(num);
}

void DimensionSelectionAction::onLoadDims() {

    const auto fileName = QFileDialog::getOpenFileName(nullptr, QObject::tr("Dimension selection"), {}, "*.txt");

    if (!fileName.isEmpty())
    {
        QFile file(fileName);

        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            disableAllDimensions();

            while (!file.atEnd())
            {
                const auto trimmedLine = file.readLine().trimmed();

                if (!trimmedLine.isEmpty())
                {
                    const auto name = QString::fromUtf8(trimmedLine);

                    if (!tryToEnableDimensionByName(name))
                    {
                        qWarning() << "Failed to select dimension (name not found): " << name;
                    }
                }
            }
        }
        else
        {
            qCritical() << "Load failed to open file: " << fileName;
        }
    }
}

bool DimensionSelectionAction::disableAllDimensions() {
    for (size_t i = 0; i < _comboBoxLineModel.rowCount(); i++) {
        _comboBoxLineModel.item(i)->setCheckState(Qt::Unchecked);
    }

    return true;
}

bool DimensionSelectionAction::tryToEnableDimensionByName(QString name) {

    for (std::size_t i{}; i < _dimNames.size(); ++i)
    {
        if (name == _dimNames[i])
        {
            _comboBoxLineModel.item(i)->setCheckState(Qt::Checked);
            return true;
        }
    }
    return false;
}


DimensionSelectionAction::Widget::Widget(QWidget* parent, DimensionSelectionAction* dimensionSelectionAction) :
    WidgetActionWidget(parent, dimensionSelectionAction)
{
    auto layout = new QHBoxLayout();

    auto dimSelectionWidget = dimensionSelectionAction->getDimensionSelection();
    auto loadWidget = dimensionSelectionAction->getLoadAction().createWidget(this);
    auto applyWidget = dimensionSelectionAction->getApplyAction().createWidget(this);

    auto numPointsWidget = dimensionSelectionAction->getNumPointsAction().createWidget(this);
    auto numDimsWidget = dimensionSelectionAction->getNumDimsAction().createWidget(this);
    auto numItemsWidget = dimensionSelectionAction->getNumItemsAction().createWidget(this);

    layout->addWidget(new QLabel("Dimensions"));
    layout->addWidget(dimSelectionWidget);
    layout->addWidget(loadWidget);
    layout->addWidget(applyWidget);

    layout->addWidget(new QLabel("#Points"));
    layout->addWidget(numPointsWidget);
    layout->addWidget(new QLabel("#Dimensions"));
    layout->addWidget(numDimsWidget);
    layout->addWidget(new QLabel("#Items"));
    layout->addWidget(numItemsWidget);

    setPopupLayout(layout);
}
