#ifndef BOARDWIDGET_H
#define BOARDWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QPainterPath>

#include <Board.h>

class BoardWidget : public QWidget
{
    Q_OBJECT

    Board &board;

public:
    BoardWidget(Board &board, QWidget *parent = nullptr);

    void paintEvent(QPaintEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;

signals:

};

#endif // BOARDWIDGET_H
