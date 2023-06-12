#ifndef BOARDWIDGET_H
#define BOARDWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>

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
    void humanPlayed(Move move);
};

#endif // BOARDWIDGET_H
