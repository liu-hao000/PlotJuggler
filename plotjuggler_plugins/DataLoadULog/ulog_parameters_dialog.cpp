#include "ulog_parameters_dialog.h"
#include "ui_ulog_parameters_dialog.h"

#include <QTableWidget>
#include <QSettings>
#include <QHeaderView>

ULogParametersDialog::ULogParametersDialog(const ULogParser& parser, QWidget* parent)
  : QDialog(parent), ui(new Ui::ULogParametersDialog)
{
  ui->setupUi(this);
  QTableWidget* table_info = ui->tableWidgetInfo;
  QTableWidget* table_params = ui->tableWidgetParams;
  QTableWidget* table_param_history = ui->tableWidgetParamHistory;
  QTableWidget* table_logs = ui->tableWidgetLogs;

  table_info->setRowCount(parser.getInfo().size());
  int row = 0;
  for (const auto& it : parser.getInfo())
  {
    table_info->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(it.first)));
    table_info->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(it.second)));
    row++;
  }
  table_info->sortItems(0);

  table_params->setRowCount(parser.getParameters().size());
  row = 0;
  for (const auto& param : parser.getParameters())
  {
    table_params->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(param.name)));
    QString value_str = (param.val_type == ULogParser::FLOAT) ?
                            QString::number(param.value.val_real) :
                            QString::number(param.value.val_int);
    table_params->setItem(row, 1, new QTableWidgetItem(value_str));
    row++;
  }
  table_params->sortItems(0);

  const auto& param_history = parser.getParameterHistory();
  table_param_history->setRowCount(param_history.size());
  row = 0;
  for (const auto& change : param_history)
  {
    QString time_str = "-";
    if (change.timestamp)
    {
      time_str = QString::number(double(*change.timestamp) / 1e6, 'f', 2);
    }

    table_param_history->setItem(row, 0, new QTableWidgetItem(time_str));
    table_param_history->setItem(row, 1,
                                 new QTableWidgetItem(QString::fromStdString(change.param.name)));
    QString value_str = (change.param.val_type == ULogParser::FLOAT) ?
                            QString::number(change.param.value.val_real) :
                            QString::number(change.param.value.val_int);
    table_param_history->setItem(row, 2, new QTableWidgetItem(value_str));
    table_param_history->setItem(
        row, 3, new QTableWidgetItem(change.is_initial ? "initial" : "change"));
    row++;
  }

  table_logs->setRowCount(parser.getLogs().size());
  row = 0;
  for (const auto& log_msg : parser.getLogs())
  {
    QString time = QString::number(0.001 * double(log_msg.timestamp / 1000), 'f', 2);
    table_logs->setItem(row, 0, new QTableWidgetItem(time));

    switch (log_msg.level)
    {
      case '0':
        table_logs->setItem(row, 1, new QTableWidgetItem("EMERGENCY"));
        break;
      case '1':
        table_logs->setItem(row, 1, new QTableWidgetItem("ALERT"));
        break;
      case '2':
        table_logs->setItem(row, 1, new QTableWidgetItem("CRITICAL"));
        break;
      case '3':
        table_logs->setItem(row, 1, new QTableWidgetItem("ERROR"));
        break;
      case '4':
        table_logs->setItem(row, 1, new QTableWidgetItem("WARNING"));
        break;
      case '5':
        table_logs->setItem(row, 1, new QTableWidgetItem("NOTICE"));
        break;
      case '6':
        table_logs->setItem(row, 1, new QTableWidgetItem("INFO"));
        break;
      case '7':
        table_logs->setItem(row, 1, new QTableWidgetItem("DEBUG"));
        break;
      default:
        table_logs->setItem(row, 1, new QTableWidgetItem(QString::number(log_msg.level)));
    }
    table_logs->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(log_msg.msg)));
    row++;
  }
}

void ULogParametersDialog::restoreSettings()
{
  QTableWidget* table_info = ui->tableWidgetInfo;
  QTableWidget* table_params = ui->tableWidgetParams;
  QTableWidget* table_param_history = ui->tableWidgetParamHistory;

  QSettings settings;
  restoreGeometry(settings.value("ULogParametersDialog/geometry").toByteArray());
  table_info->horizontalHeader()->restoreState(
      settings.value("ULogParametersDialog/info/state").toByteArray());
  table_params->horizontalHeader()->restoreState(
      settings.value("ULogParametersDialog/params/state").toByteArray());
  table_param_history->horizontalHeader()->restoreState(
      settings.value("ULogParametersDialog/params_history/state").toByteArray());

  table_info->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
  table_info->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Interactive);

  table_params->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
  table_params->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Interactive);
  table_param_history->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
  table_param_history->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Interactive);
  table_param_history->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Interactive);
  table_param_history->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Interactive);
}

ULogParametersDialog::~ULogParametersDialog()
{
  QTableWidget* table_info = ui->tableWidgetInfo;
  QTableWidget* table_params = ui->tableWidgetParams;
  QTableWidget* table_param_history = ui->tableWidgetParamHistory;

  QSettings settings;
  settings.setValue("ULogParametersDialog/geometry", this->saveGeometry());
  settings.setValue("ULogParametersDialog/info/state", table_info->horizontalHeader()->saveState());
  settings.setValue("ULogParametersDialog/params/state",
                    table_params->horizontalHeader()->saveState());
  settings.setValue("ULogParametersDialog/params_history/state",
                    table_param_history->horizontalHeader()->saveState());

  delete ui;
}
