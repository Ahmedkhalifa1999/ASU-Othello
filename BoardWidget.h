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

    /**
     * @brief board
     */
    Board &board;

public:
    /**
     * @brief BoardWidget
     * @param board
     * @param parent
     */
    BoardWidget(Board &board, QWidget *parent = nullptr);

    /**
     * @brief paintEvent
     * @param event
     */
    void paintEvent(QPaintEvent *event) override;

    /**
     * @brief mousePressEvent
     * @param event
     */
    void mousePressEvent(QMouseEvent *event) override;

signals:
    /**
     * @brief humanPlayed
     * @param move
     */
    void humanPlayed(Move move);
};

#endif // BOARDWIDGET_H
