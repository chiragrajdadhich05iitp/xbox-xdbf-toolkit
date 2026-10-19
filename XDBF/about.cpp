/**
 * @file about.cpp
 * @brief About dialog implementation for Xbox XDBF Explorer
 * Distributed under the terms of the GNU General Public License v3.0.
 */

#include "about.h"
#include "ui_about.h"
#include <QDesktopServices>
#include <QUrl>

About::About(QWidget *parent) : QDialog(parent), ui(new Ui::About)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);

    setWindowTitle("About Xbox XDBF Explorer");
    setFixedSize(sizeHint());
}

About::~About()
{
    delete ui;
}

void About::on_pushButton_clicked()
{
    // Free60 Xbox Architecture & Hardware Documentation
    QDesktopServices::openUrl(QUrl("https://free60.org"));
}

void About::on_pushButton_3_clicked()
{
    // Direct link to your repository
    QDesktopServices::openUrl(QUrl("https://github.com/chiragrajdadhich05iitp/xbox-xdbf-explorer"));
}

void About::on_pushButton_2_clicked()
{
    // GNU GPL v3.0 License reference
    QDesktopServices::openUrl(QUrl("https://www.gnu.org/licenses/gpl-3.0.html"));
}
