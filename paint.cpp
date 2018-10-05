#include "paint.h"
#include "ui_paint.h"

Paint::Paint(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Paint)
{
    ui->setupUi(this);

    scene = new paintScene();
    ui->graphicsView->setScene(scene);
    ui->graphicsView->setRenderHint(QPainter::HighQualityAntialiasing);

    timer = new QTimer();
    connect(timer, &QTimer::timeout, this, &Paint::slotTimer);
    timer->start(100);
}

Paint::~Paint()
{
    delete ui;
}

void Paint::slotTimer()
{
    timer->stop();
    scene->setSceneRect(0,0, ui->graphicsView->width() - 20, ui->graphicsView->height() - 20);
}

void Paint::resizeEvent(QResizeEvent *event)
{
    timer->start(100);
    QWidget::resizeEvent(event);
}
